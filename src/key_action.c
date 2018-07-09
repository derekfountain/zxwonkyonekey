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
#include "utils.h"
#include "action.h"
#include "runner.h"
#include "game_state.h"

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
GAME_ACTION test_for_direction_change( void* data )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address = NULL;

  uint8_t     xpos = get_runner_xpos();
  uint8_t     ypos = get_runner_ypos();

  /* If the user pressed the button, turn him */
  if( game_state->key_pressed && ! game_state->key_processed ) {
    game_state->key_processed = 1;
    return TOGGLE_DIRECTION;
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
  
  if( attr_address && ((*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE) )
    return TOGGLE_DIRECTION;

  return NO_ACTION;
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
GAME_ACTION test_for_start_jump( void* data )
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
  if( RUNNER_JUMPING( get_runner_jump_offset() ) )
    return NO_ACTION;

  /*
   * If the key hasn't been pressed, or it has but it's already been processed,
   * there's no jump to even potentially kick off
   */
  if( !game_state->key_pressed || game_state->key_processed )
    return NO_ACTION;

  /* Is the cell directly below him a trampoline block? */
  attr_address = zx_pxy2aaddr( xpos, ypos+8  );
  if( (*attr_address & ATTR_MASK_PAPER) != PAPER_RED ) {

    /* No, so check the block below and to the right, which the sprite might have rotated into */
    if( (MODULO8( xpos ) < 3) ) {
      /* Sprite hasn't rotated far enough to stray onto next block */
      return NO_ACTION;
    }

   attr_address = zx_pxy2aaddr( xpos+8, ypos+8  );
   if( (*attr_address & ATTR_MASK_PAPER) != PAPER_RED ) {
     /* Block the sprite is rotated onto isn't a jump block either. */
     return NO_ACTION;
   }

   /* His heel or toe is on top of a jump block so drop through */
  }
  else {
    /* He's directly on top of a jump block, so drop through */
  }

  game_state->key_processed = 1;
  return JUMP;
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
GAME_ACTION test_for_falling( void* data )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address = NULL;

  uint8_t     xpos = get_runner_xpos();
  uint8_t     ypos = get_runner_ypos();

  /* Are we in the middle of a jump? If so, no action */
  if( RUNNER_JUMPING( get_runner_jump_offset() ) )
    return NO_ACTION;

  /*
   * TODO I can probably optimise this for space but pulling out
   * the repeated zx_pxy2aaddr() calls.
   */

  if( get_runner_facing() == RIGHT ) {

    /* Is the cell below him solid? If so, he's supported */
    attr_address = zx_pxy2aaddr( xpos, ypos+8 );
    if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
      return NO_ACTION;
    }

    /*
     * If he's facing right and hasn't rotated onto the cell to his right (in front of him)
     * then he's only supported by the cell directly underneath him which we know isn't solid.
     */
    if( MODULO8(xpos) < 3 ) {
      return MOVE_DOWN;
    }

    /*
     * He's rotated onto the cell in front of him, to the right. If that cell is solid
     * then his toes are supported
     */
    attr_address = zx_pxy2aaddr( xpos+8, ypos+8 );
    if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
      return MOVE_DOWN;
    }
    else {
      return NO_ACTION;
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
      return NO_ACTION;
    }
    else {
      return MOVE_DOWN;
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
GAME_ACTION test_for_killer( void* data )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address;

  uint8_t     xpos = get_runner_xpos();
  uint8_t     ypos = get_runner_ypos();

  /* Are we in the middle of a jump? If so, no action */
  if( RUNNER_JUMPING( get_runner_jump_offset() ) )
    return NO_ACTION;

  /* Are we on a killer block? If not, no action */
  attr_address = zx_pxy2aaddr( xpos, ypos+8  );
  
  if( (*attr_address & ATTR_MASK_PAPER) != PAPER_BLUE )
  {
    /* If the cell below isn't a killer, and he's aligned right on top of it, he's fine */
    if( MODULO8( xpos ) == 0 )
      return NO_ACTION;

    /* He's moved onto the next character cell. Is that a killer? */
    if( get_runner_facing() == RIGHT )
      attr_address = zx_pxy2aaddr( xpos+8, ypos+8 );
  
    if( (*attr_address & ATTR_MASK_PAPER) == PAPER_BLUE )
      return DIE;
    else
      return NO_ACTION;
  }

  return DIE;
}


GAME_ACTION test_for_finish( void* data )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address;

  uint8_t     xpos = get_runner_xpos();
  uint8_t     ypos = get_runner_ypos();

  /* Are we in the middle of a jump? If so, no action */
  if( RUNNER_JUMPING( get_runner_jump_offset() ) )
    return NO_ACTION;

  /* If he's not on a character cell boundary he can't be up against a wall */
  if( MODULO8( xpos ) == 0 ) {

    /* Pick up the attributes of the char cell he's facing and about to move into */
    if( get_runner_facing() == RIGHT )
      attr_address = zx_pxy2aaddr( xpos+8, ypos );
    else
      attr_address = zx_pxy2aaddr( xpos-8, ypos );
  
    if( (*attr_address & ATTR_MASK_PAPER) == PAPER_YELLOW )
      return FINISH;
  }

  return NO_ACTION;
}

