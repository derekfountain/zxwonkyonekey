/*
 * Wonky One Key, a ZX Spectrum game featuring a single control key
 * Copyright (C) 2018 Derek Fountain
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <arch/zx.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "action.h"
#include "runner.h"
#include "game_state.h"
#include "tracetable.h"
#include "int.h"

/***
 *      _______             _             
 *     |__   __|           (_)            
 *        | |_ __ __ _  ___ _ _ __   __ _ 
 *        | | '__/ _` |/ __| | '_ \ / _` |
 *        | | | | (_| | (__| | | | | (_| |
 *        |_|_|  \__,_|\___|_|_| |_|\__, |
 *                                   __/ |
 *                                  |___/ 
 *
 * Key action tracing. Bit scattered, but can be handy.
 */

typedef enum _key_action_tracetype
{
  TEST_DIR_CHG,
  TEST_JUMP,
  TEST_FALL,
  TEST_KILLER,
  TEST_FINISH,
} KEY_ACTION_TRACETYPE;

typedef struct _key_action_trace
{
  uint16_t               ticker;
  KEY_ACTION_TRACETYPE   tracetype;
} KEY_ACTION_TRACE;

#define KEY_ACTION_TRACE_ENTRIES   25
#define KEY_ACTION_TRACETABLE_SIZE ((size_t)sizeof(KEY_ACTION_TRACE)*KEY_ACTION_TRACE_ENTRIES)

KEY_ACTION_TRACE* key_action_tracetable = TRACING_UNINITIALISED;
KEY_ACTION_TRACE* key_action_next_trace = 0xFFFF;

void key_action_add_trace( KEY_ACTION_TRACE* ka_ptr )
{
  memcpy(key_action_next_trace, ka_ptr, sizeof(KEY_ACTION_TRACE));

  key_action_next_trace = (void*)((uint8_t*)key_action_next_trace + sizeof(KEY_ACTION_TRACE));

  if( key_action_next_trace == (void*)((uint8_t*)key_action_tracetable+KEY_ACTION_TRACETABLE_SIZE) )
    key_action_next_trace = key_action_tracetable;
}

void KEY_ACTION_TRACE_CREATE( KEY_ACTION_TRACETYPE ttype )
{
  if( key_action_tracetable != TRACING_INACTIVE ) {
    KEY_ACTION_TRACE  ka;		  
    ka.ticker       = GET_TICKER;			
    ka.tracetype    = ttype;
    key_action_add_trace(&ka);			
  }						
}

void init_key_action_trace(void)
{
  if( key_action_tracetable == TRACING_UNINITIALISED )
    key_action_tracetable = key_action_next_trace = allocate_tracememory(KEY_ACTION_TRACETABLE_SIZE);
}


/*
TODO

Replace PAPER_WHITE with a value from the level data (empty sky).
Also green (walls) and red (killer)
Level data needs to be in the game state.

 */

/***
 *      _____  _               _   _                _____ _                            
 *     |  __ \(_)             | | (_)              / ____| |                           
 *     | |  | |_ _ __ ___  ___| |_ _  ___  _ __   | |    | |__   __ _ _ __   __ _  ___ 
 *     | |  | | | '__/ _ \/ __| __| |/ _ \| '_ \  | |    | '_ \ / _` | '_ \ / _` |/ _ \
 *     | |__| | | | |  __| (__| |_| | (_) | | | | | |____| | | | (_| | | | | (_| |  __/
 *     |_____/|_|_|  \___|\___|\__|_|\___/|_| |_|  \_____|_| |_|\__,_|_| |_|\__, |\___|
 *                                                                           __/ |     
 *                                                                          |___/      
 * Test for direction change.
 *
 * Runner changes direction if he hits a wall or the user
 * presses the control key.
 *
 * Returns TOGGLE_DIRECTION if the runner's direction is to
 * be reversed.
 */
PROCESSING_FLAG test_for_direction_change( void* data, GAME_ACTION* output_action )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address = NULL;

  uint8_t     xpos = get_runner_xpos();
  uint8_t     ypos = get_runner_ypos();

  /* If the user pressed the button, turn him */
  if( game_state->key_pressed && ! game_state->key_processed ) {
    game_state->key_processed = 1;
    *output_action = TOGGLE_DIRECTION;

    KEY_ACTION_TRACE_CREATE( TEST_DIR_CHG );

    return KEEP_PROCESSING;
  }

  /*
   * Does he need to bounce off a wall he's facing?
   *
   * The complication is that the sprite is 6 pixels wide, left aligned
   * within the 8 pixel wide cell. So if he's facing right he needs to be
   * on, say, pixel 2, or pixel 10, to be up against the wall he's facing.
   * Facing left is more straightforward.
   *
   * If he's up against the edge of a cell, pick up the attributes of the
   * cell he's facing and about to move into. If it's a wall, bounce him.
   */
  if( get_runner_facing() == RIGHT ) {

    if( MODULO8( xpos ) == 2 ) {
      attr_address = zx_pxy2aaddr( xpos+8, ypos );
    }
  }
  else {  /* He's facing left */

    if( MODULO8( xpos ) == 0 ) {
      attr_address = zx_pxy2aaddr( xpos-8, ypos );
    }
  }
  
  if( attr_address && ((*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE) ) {
    *output_action = TOGGLE_DIRECTION;
    return KEEP_PROCESSING;
  }

  *output_action = NO_ACTION;
  return KEEP_PROCESSING;
}



/***
 *           _                      ___  
 *          | |                    |__ \ 
 *          | |_   _ _ __ ___  _ __   ) |
 *      _   | | | | | '_ ` _ \| '_ \ / / 
 *     | |__| | |_| | | | | | | |_) |_|  
 *      \____/ \__,_|_| |_| |_| .__/(_)  
 *                            | |        
 *                            |_|        
 * Test for making runner jump.
 *
 * Runner jumps if:
 *
 *  he's not already jumping; and
 *  part of him is on a trampoline block; and
 *  player hits the control key
 */
PROCESSING_FLAG test_for_start_jump( void* data, GAME_ACTION* output_action )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address;
  uint8_t     on_jump_block = 0;

  uint8_t     xpos = get_runner_xpos();
  uint8_t     ypos = get_runner_ypos();

  /*
   * Are we already jumping? If so, no action. Don't process any keypress so
   * it can still turn him around in midair.
   */
  if( RUNNER_JUMPING( get_runner_jump_offset() ) ) {
    *output_action = NO_ACTION;
    return KEEP_PROCESSING;
  }

  /*
   * If the key hasn't been pressed, or it has but it's already been processed,
   * there's no jump to even potentially kick off
   */
  if( !game_state->key_pressed || game_state->key_processed ) {
    *output_action = NO_ACTION;
    return KEEP_PROCESSING;
  }

  /* Is the cell directly below him a trampoline block? */
  attr_address = zx_pxy2aaddr( xpos, ypos+8  );
  if( (*attr_address & ATTR_MASK_PAPER) != PAPER_RED ) {

    /* No, so check the block below and to the right, which the sprite might have rotated into */
    if( (MODULO8( xpos ) < 3) ) {
      /* Sprite hasn't rotated far enough to stray onto next block */
      *output_action = NO_ACTION;
      return KEEP_PROCESSING;
    }

   attr_address = zx_pxy2aaddr( xpos+8, ypos+8  );
   if( (*attr_address & ATTR_MASK_PAPER) != PAPER_RED ) {
     /* Block the sprite is rotated onto isn't a jump block either. */
     *output_action = NO_ACTION;
     return KEEP_PROCESSING;
   }

   /* His heel or toe is on top of a jump block so drop through */
  }
  else {
    /* He's directly on top of a jump block, so drop through */
  }

  game_state->key_processed = 1;
  *output_action = JUMP;
  return STOP_PROCESSING;
}




/***
 *      ______    _ _ ___  
 *     |  ____|  | | |__ \ 
 *     | |__ __ _| | |  ) |
 *     |  __/ _` | | | / / 
 *     | | | (_| | | ||_|  
 *     |_|  \__,_|_|_|(_)  
 *                         
 * Test for making runner fall:
 *
 * Runner falls if:
 *
 *  he's not in the middle of a jump; and
 *  the attribute cell underneath him is clear; and
 *  he's aligned on top of that cell
 *
 * If he's facing left he must move the 2 pixels of alignment on the
 * right side of the sprite.
 *
 * He might have fallen onto the block he's on, so it's possible he's
 * facing right and is on left pixel 0, 1 or 2 of the cell. In which
 * case fall through.
 */
PROCESSING_FLAG test_for_falling( void* data, GAME_ACTION* output_action )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address = NULL;

  uint8_t     xpos = get_runner_xpos();
  uint8_t     ypos = get_runner_ypos();

  /* Are we in the middle of a jump? If so, no action */
  if( RUNNER_JUMPING( get_runner_jump_offset() ) ) {
    *output_action = NO_ACTION;
    return KEEP_PROCESSING;
  }

  /*
   * TODO I can probably optimise this for space but pulling out
   * the repeated zx_pxy2aaddr() calls.
   */

  if( get_runner_facing() == RIGHT ) {

    /* Is the cell below him solid? If so, he's supported */
    attr_address = zx_pxy2aaddr( xpos, ypos+8 );
    if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
      *output_action = NO_ACTION;
      return KEEP_PROCESSING;
    }

    /*
     * If he's facing right and hasn't rotated onto the cell to his right (in front of him)
     * then he's only supported by the cell directly underneath him which we know isn't solid.
     */
    if( MODULO8(xpos) < 3 ) {
      *output_action = MOVE_DOWN;
      return STOP_PROCESSING;
    }

    /*
     * He's rotated onto the cell in front of him, to the right. If that cell is solid
     * then his toes are supported
     */
    attr_address = zx_pxy2aaddr( xpos+8, ypos+8 );
    if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
      *output_action = MOVE_DOWN;
      return STOP_PROCESSING;
    }
    else {
      *output_action = NO_ACTION;
      return KEEP_PROCESSING;
    }
  }
  else {  /* Facing left */

    if( MODULO8( xpos ) < 3 ) {

      /* If he's over to the left side of his cell his heels can't be supported */
      attr_address = zx_pxy2aaddr( xpos, ypos+8  );
    }
    else {

      /* If he's over on the right side of his cell, his heels are supported */
      attr_address = zx_pxy2aaddr( xpos+8, ypos+8  );
    }

    if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
      *output_action = NO_ACTION;
      return KEEP_PROCESSING;
    }
    else {
      *output_action = MOVE_DOWN;
      return STOP_PROCESSING;
    }
  }
}




/***
 *     __          __   _ _            _               _          _    _ _ _         ___  
 *     \ \        / /  | | |          | |             | |        | |  (_| | |       |__ \ 
 *      \ \  /\  / __ _| | | _____  __| |   ___  _ __ | |_ ___   | | ___| | | ___ _ __ ) |
 *       \ \/  \/ / _` | | |/ / _ \/ _` |  / _ \| '_ \| __/ _ \  | |/ | | | |/ _ | '__/ / 
 *        \  /\  | (_| | |   |  __| (_| | | (_) | | | | || (_) | |   <| | | |  __| | |_|  
 *         \/  \/ \__,_|_|_|\_\___|\__,_|  \___/|_| |_|\__\___/  |_|\_|_|_|_|\___|_| (_)  
 *                                                                                        
 *                                                                                        
 */
PROCESSING_FLAG test_for_killer( void* data, GAME_ACTION* output_action )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address;

  uint8_t     xpos = get_runner_xpos();
  uint8_t     ypos = get_runner_ypos();

  /* Are we in the middle of a jump? If so, no action */
  if( RUNNER_JUMPING( get_runner_jump_offset() ) ) {
    *output_action = NO_ACTION;
    return KEEP_PROCESSING;
  }

  /* Are we on a killer block? If not, no action */
  attr_address = zx_pxy2aaddr( xpos, ypos+8  );
  
  if( (*attr_address & ATTR_MASK_PAPER) != PAPER_BLUE )
  {
    /* If the cell below isn't a killer, and he's aligned right on top of it, he's fine */
    if( MODULO8( xpos ) == 0 ) {
      *output_action = NO_ACTION;
      return KEEP_PROCESSING;
    }

    /* He's moved onto the next character cell. Is that a killer? */
    if( get_runner_facing() == RIGHT )
      attr_address = zx_pxy2aaddr( xpos+8, ypos+8 );
  
    if( (*attr_address & ATTR_MASK_PAPER) == PAPER_BLUE ) {
      *output_action = DIE;
      return STOP_PROCESSING;
    }
    else {
      *output_action = NO_ACTION;
      return KEEP_PROCESSING;
    }
  }
  else {
    *output_action = DIE;
    return STOP_PROCESSING;
  }
}

/***
 *      ______ _       _     _    ___  
 *     |  ____(_)     (_)   | |  |__ \ 
 *     | |__   _ _ __  _ ___| |__   ) |
 *     |  __| | | '_ \| / __| '_ \ / / 
 *     | |    | | | | | \__ | | | |_|  
 *     |_|    |_|_| |_|_|___|_| |_(_)  
 *                                     
 *                                     
 */
PROCESSING_FLAG test_for_finish( void* data, GAME_ACTION* output_action )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address;

  uint8_t     xpos = get_runner_xpos();
  uint8_t     ypos = get_runner_ypos();

  /* Are we in the middle of a jump? If so, no action */
  if( RUNNER_JUMPING( get_runner_jump_offset() ) ) {
    *output_action = NO_ACTION;
    return KEEP_PROCESSING;
  }

  /* If he's not on a character cell boundary he can't be up against a wall */
  /*
   * TODO This won't work if he approaches the finish from the left side
   * i.e. facing right
   */
  if( MODULO8( xpos ) == 0 ) {

    /* Pick up the attributes of the char cell he's facing and about to move into */
    if( get_runner_facing() == RIGHT )
      attr_address = zx_pxy2aaddr( xpos+8, ypos );
    else
      attr_address = zx_pxy2aaddr( xpos-8, ypos );
  
    if( (*attr_address & ATTR_MASK_PAPER) == PAPER_YELLOW ) {
      *output_action = FINISH;
      return STOP_PROCESSING;
    }
  }

  *output_action = NO_ACTION;
  return KEEP_PROCESSING;
}

