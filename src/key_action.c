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
#include <input.h>
#include <sound.h>
#include <string.h>
#include "utils.h"
#include "action.h"
#include "runner.h"
#include "teleporter.h"
#include "door.h"
#include "game_state.h"
#include "tracetable.h"
#include "int.h"
#include "sound.h"
#include "levels.h"

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
  TEST_DIR_CHG_BOUNCE,
  TEST_DIR_CHG_KEY,
  TEST_JUMP_ON_BLOCK,
  TEST_JUMP_PARTIAL_ON_BLOCK,
  TEST_FALL_RIGHT_UNROTATED,
  TEST_FALL_RIGHT_NO_TOE_SUPPORT,
  TEST_FALL_LEFT_HEEL_SUPPORT,
  ENTER_TELEPORTER,
  JUST_TELEPORTED,
  SKIP_DIR_CHG_TELEPORTER,
  TEST_FINISH,
  CONSUMED_SLOWDOWN,
  OPENED_DOOR,
  SLOWDOWN_EXPIRED,
} KEY_ACTION_TRACETYPE;

typedef struct _key_action_trace
{
  uint16_t               ticker;
  KEY_ACTION_TRACETYPE   tracetype;

  /* BE:LITERAL:START
  union
  {
    n16 dec valid "tracetype==TEST_DIR_CHG_BOUNCE"         "bounced"
    n16 dec valid "tracetype==TEST_DIR_CHG_KEY"            "key pressed"
    n16 dec valid "tracetype==TEST_JUMP_PARTIAL_ON_BLOCK"  "mod(8) xpos"
    n16 dec valid "tracetype==TEST_FALL_RIGHT_NO_TOE_SUPPORT"  "mod(8) xpos"
    n16 dec valid "tracetype==TEST_FALL_LEFT_HEEL_SUPPORT"  "mod(8) xpos"
    n16 dec valid "tracetype==ENTER_TELEPORTER"  "toggle direction"
  }    
  BE:LITERAL:END */

  uint16_t /* BE:ignore */data;  
} KEY_ACTION_TRACE;

/* BE:PICKUPDEF */
#define KEY_ACTION_TRACE_ENTRIES   100
#define KEY_ACTION_TRACETABLE_SIZE ((size_t)sizeof(KEY_ACTION_TRACE)*KEY_ACTION_TRACE_ENTRIES)

TRACE_FN( key_action, KEY_ACTION_TRACE, KEY_ACTION_TRACETABLE_SIZE )

/*
 * Filling in a blank trace entry is normally done with a macro,
 * but since this one is called several times a function is more
 * economical.
 */
void KEY_ACTION_TRACE_CREATE( KEY_ACTION_TRACETYPE ttype, uint16_t d )
{
  if( key_action_tracetable != TRACING_INACTIVE ) {
    KEY_ACTION_TRACE  ka;                 
    ka.ticker       = GET_TICKER;                       
    ka.tracetype    = ttype;
    ka.data         = d;
    key_action_add_trace(&ka);                  
  }                                             
}

void init_key_action_trace(void)
{
  key_action_tracetable = key_action_next_trace = allocate_tracememory(KEY_ACTION_TRACETABLE_SIZE);
}

/*
 * See comment in test_for_teleporter() to see what this is for
 */
uint8_t just_teleported = 0;


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
 * Runner changes direction if the user presses the control key.
 *
 * Returns TOGGLE_DIRECTION if the runner's direction is to
 * be reversed.
 */
PROCESSING_FLAG test_for_direction_change( void* data, GAME_ACTION* output_action )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address = NULL;

  uint8_t     xpos = GET_RUNNER_XPOS;
  uint8_t     ypos = GET_RUNNER_YPOS;

  /* If the user pressed the button, turn him */
  if( game_state->key_pressed && ! game_state->key_processed ) {

    /*
     * Direction change keypress has been hit. But before this happens
     * check he's not standing right on the trigger point of a teleporter.
     * If he is things go wrong: this code will change his direction,
     * then the teleporter code might also change his direction, which
     * means he comes out of the teleporter the wrong side. So don't
     * allow the direction change to happen at this moment, just keep
     * the flags so next time round the cycle when he's moved off the
     * teleporter it's safe to change direction.
     */
    LEVEL_DATA* level_data            = game_state->current_level;
    TELEPORTER_DEFINITION* teleporter = level_data->teleporters;

    if( teleporter )
    {
      while( (teleporter->end_1_x || teleporter->end_1_y) ) {

        if( (xpos == teleporter->end_1_x && ypos == teleporter->end_1_y) ||
            (xpos == teleporter->end_2_x && ypos == teleporter->end_2_y) )
        {
          KEY_ACTION_TRACE_CREATE( SKIP_DIR_CHG_TELEPORTER, (uint16_t)teleporter );

          *output_action = NO_ACTION;
          return KEEP_PROCESSING;
        }
        teleporter++;
      }
    }

    game_state->key_processed = 1;
    *output_action = TOGGLE_DIRECTION;

    KEY_ACTION_TRACE_CREATE( TEST_DIR_CHG_KEY, 0 );

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
 *  part of him is on a jumper block; and
 *  player hits the control key
 */
PROCESSING_FLAG test_for_start_jump( void* data, GAME_ACTION* output_action )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address;
  uint8_t     on_jump_block = 0;
  uint8_t     jumper_attribute;

  uint8_t     xpos = GET_RUNNER_XPOS;
  uint8_t     ypos = GET_RUNNER_YPOS;

  /*
   * Are we already jumping? If so, no action. Don't process any keypress so
   * it can still turn him around in midair.
   */
  if( RUNNER_JUMPING( GET_RUNNER_JUMP_OFFSET ) ) {
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

  /* Find the attribute colour of a jumper block in this level */
  jumper_attribute = game_state->current_level->jumper_att;

  /* Is the cell directly below him a trampoline block? */
  attr_address = zx_pxy2aaddr( xpos, ypos+8  );
  if( *attr_address != jumper_attribute ) {

    /* No, so check the block below and to the right, which the sprite might have rotated into */
    if( MODULO8( xpos ) < 3 ) {
      /* Sprite hasn't rotated far enough to stray onto next block */
      *output_action = NO_ACTION;
      return KEEP_PROCESSING;
    }

    attr_address = zx_pxy2aaddr( xpos+8, ypos+8  );
    if( *attr_address != jumper_attribute ) {
      /* Block the sprite is rotated onto isn't a jump block either. */
      *output_action = NO_ACTION;
      return KEEP_PROCESSING;
    }

    /* His heel or toe is on top of a jump block so drop through */
    KEY_ACTION_TRACE_CREATE( TEST_JUMP_PARTIAL_ON_BLOCK, MODULO8( xpos ) );
  }
  else {
    /* He's directly on top of a jump block, so drop through */

    KEY_ACTION_TRACE_CREATE( TEST_JUMP_ON_BLOCK, 0 );
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
 *
 * The collision detection code could probably be used instead of
 * this, saving a few bytes.
 */
PROCESSING_FLAG test_for_falling( void* data, GAME_ACTION* output_action )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address = NULL;
  uint8_t     background_attribute;

  uint8_t     xpos = GET_RUNNER_XPOS;
  uint8_t     ypos = GET_RUNNER_YPOS;

  /* Are we in the middle of a jump? If so, no action */
  if( RUNNER_JUMPING( GET_RUNNER_JUMP_OFFSET ) ) {
    *output_action = NO_ACTION;
    return KEEP_PROCESSING;
  }

  /* Find the attribute colour of a background block in this level */
  background_attribute = game_state->current_level->background_att;

  /*
   * I can probably optimise this for space but pulling out
   * the repeated zx_pxy2aaddr() calls.
   */

  /* Is the cell below him solid? If so, he's supported */
  attr_address = zx_pxy2aaddr( xpos, ypos+8 );
  if( *attr_address != background_attribute ) {
    *output_action = NO_ACTION;
    return KEEP_PROCESSING;
  }

  if( GET_RUNNER_FACING == RIGHT ) {

    /*
     * If he's facing right and hasn't rotated onto the cell to his right (in front of him)
     * then he's only supported by the cell directly underneath him which we know isn't solid.
     */
    if( MODULO8(xpos) < 3 ) {
      *output_action = MOVE_DOWN;
      KEY_ACTION_TRACE_CREATE( TEST_FALL_RIGHT_UNROTATED, MODULO8(xpos) );
      return STOP_PROCESSING;
    }

    /*
     * He's rotated onto the cell in front of him, to the right. If that cell is solid
     * then his toes are supported
     */
    attr_address = zx_pxy2aaddr( xpos+8, ypos+8 );
    if( *attr_address != background_attribute ) {
      *output_action = NO_ACTION;
      return KEEP_PROCESSING;
    }
    else {
      *output_action = MOVE_DOWN;
      KEY_ACTION_TRACE_CREATE( TEST_FALL_RIGHT_NO_TOE_SUPPORT, 0 );
      return STOP_PROCESSING;
    }
  }
  else {  /* Facing left */

    /*
     * If he's over on the right side of his cell, check if his heels are supported.
     * If not he should fall.
     */
    if( MODULO8( xpos ) >= 3 ) {

      attr_address = zx_pxy2aaddr( xpos+8, ypos+8  );

      KEY_ACTION_TRACE_CREATE( TEST_FALL_LEFT_HEEL_SUPPORT, MODULO8( xpos ) );

      if( *attr_address != background_attribute ) {
        *output_action = NO_ACTION;
        return KEEP_PROCESSING;
      }
    }

    *output_action = MOVE_DOWN;
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

  uint8_t     xpos = GET_RUNNER_XPOS;
  uint8_t     ypos = GET_RUNNER_YPOS;

#define CHEAT_MODE 0
#if CHEAT_MODE
  /* Check for cheat key */
  if( in_key_pressed( IN_KEY_SCANCODE_q ) ) {
    *output_action = FINISH;
    return STOP_PROCESSING;
  }
  if( in_key_pressed( IN_KEY_SCANCODE_w ) ) {
    *output_action = LOSE;
    return STOP_PROCESSING;
  }
#endif

  /* Are we in the middle of a jump? If so, no action */
  if( RUNNER_JUMPING( GET_RUNNER_JUMP_OFFSET ) ) {
    *output_action = NO_ACTION;
    return KEEP_PROCESSING;
  }

  /* If he's not on a character cell boundary he can't be up against a wall */
  if( MODULO8( xpos ) == 0 ) {

    /* Pick up the attributes of the char cell he's facing and about to move into */
    if( GET_RUNNER_FACING == RIGHT )
      attr_address = zx_pxy2aaddr( xpos+8, ypos );
    else
      attr_address = zx_pxy2aaddr( xpos-8, ypos );
  
    if( *attr_address == FINISH_ATT ) {
      *output_action = FINISH;
      return STOP_PROCESSING;
    }
  }

  *output_action = NO_ACTION;
  return KEEP_PROCESSING;
}




/***
 *      _______   _                       _          ___  
 *     |__   __| | |                     | |        |__ \ 
 *        | | ___| | ___ _ __   ___  _ __| |_ ___ _ __ ) |
 *        | |/ _ | |/ _ | '_ \ / _ \| '__| __/ _ | '__/ / 
 *        | |  __| |  __| |_) | (_) | |  | ||  __| | |_|  
 *        |_|\___|_|\___| .__/ \___/|_|   \__\___|_| (_)  
 *                      | |                               
 *                      |_|                               
 */

PROCESSING_FLAG test_for_teleporter( void* data, GAME_ACTION* output_action )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  LEVEL_DATA* level_data = game_state->current_level;
  PROCESSING_FLAG return_value = KEEP_PROCESSING;

  uint8_t     xpos = GET_RUNNER_XPOS;
  uint8_t     ypos = GET_RUNNER_YPOS;

  TELEPORTER_DEFINITION* teleporter = level_data->teleporters;

  *output_action = NO_ACTION;

  /*
   * There's an issue here with the slowdown code. If slowdown is on then he only
   * moves every other cycle. That means if he teleports when slowdown is on his
   * x,y position is updated to the destination end of the teleporter, but then
   * he doesn't move. Next cycle this code finds him at the teleporter trigger
   * location and he's teleported back. He still doesn't move for this cycle so
   * we go round again. He's stuck in an endless teleport sequence!
   * The fix is to keep a counter saying he's just teleported. Start it at 2 then
   * decrement it and don't do the teleport sequence. That frees up 2 cycles to move
   * his location by a pixel. Even if slowdown is active, one of those cycles will
   * see him move by one pixel. That way he leaves the teleport trigger point and
   * then it all works again.
   */
  if( just_teleported )
  {
    KEY_ACTION_TRACE_CREATE( JUST_TELEPORTED, (*output_action == TOGGLE_DIRECTION) );
    just_teleported--;
  }
  else
  {

    while( teleporter && (teleporter->end_1_x || teleporter->end_1_y) ) {

      if( xpos == teleporter->end_1_x && ypos == teleporter->end_1_y ) {

        SET_RUNNER_XPOS( teleporter->end_2_x );
        SET_RUNNER_YPOS( teleporter->end_2_y );
        just_teleported = 2;

        if( teleporter->change_direction ) {
          *output_action = TOGGLE_DIRECTION;
        }

        /* Play effect immediately otherwise he starts to emerge from the teleporter */
        play_beepfx_sound_immediate(BEEPFX_SELECT_3);

        KEY_ACTION_TRACE_CREATE( ENTER_TELEPORTER, (*output_action == TOGGLE_DIRECTION) );

        return_value = STOP_PROCESSING;

        break;

      } else if( xpos == teleporter->end_2_x && ypos == teleporter->end_2_y ) {

        SET_RUNNER_XPOS( teleporter->end_1_x );
        SET_RUNNER_YPOS( teleporter->end_1_y );
        just_teleported = 1;

        if( teleporter->change_direction ) {
          *output_action = TOGGLE_DIRECTION;
        }

        /* Play effect immediately otherwise he starts to emerge from the teleporter */
        play_beepfx_sound_immediate(BEEPFX_SELECT_3);

        KEY_ACTION_TRACE_CREATE( ENTER_TELEPORTER, (*output_action == TOGGLE_DIRECTION) );

        return_value = STOP_PROCESSING;

        break;

      }

      teleporter++;
    }
  }
  return return_value;
}




/***
 *       _____ _                  _                       _____ _ _ _ ___  
 *      / ____| |                | |                     |  __ (_| | |__ \ 
 *     | (___ | | _____      ____| | _____      ___ __   | |__) _| | |  ) |
 *      \___ \| |/ _ \ \ /\ / / _` |/ _ \ \ /\ / | '_ \  |  ___| | | | / / 
 *      ____) | | (_) \ V  V | (_| | (_) \ V  V /| | | | | |   | | | ||_|  
 *     |_____/|_|\___/ \_/\_/ \__,_|\___/ \_/\_/ |_| |_| |_|   |_|_|_|(_)  
 *                                                                         
 *                                                                         
 */
PROCESSING_FLAG test_for_slowdown_pill( void* data, GAME_ACTION* output_action )
{
  GAME_STATE* game_state = (GAME_STATE*)data;

  *output_action = NO_ACTION;

  /* Skip everything if this level doesn't have slowdown pills */
  if( game_state->current_level->slowdowns )
  {
    SLOWDOWN* slowdown = game_state->current_level->slowdowns;

    uint8_t xpos = GET_RUNNER_XPOS;
    uint8_t ypos = GET_RUNNER_YPOS;

    /* Loop over slowdown pills */
    while( IS_VALID_SLOWDOWN(slowdown) )
    {
      /*
       * If the pill has been consumed and is active, reduce the timer. If that's got
       * to zero, reinstate the pill.
       *
       * This needs to happen even if the last pill has been consumed because the timeout
       * for the active one still needs to work.
       */
      if( !IS_COLLECTABLE_AVAILABLE( slowdown->collectable ) )
      {
        DECREMENT_COLLECTABLE_TIMER( slowdown->collectable );

        if( COLLECTABLE_TIMER_EXPIRED( slowdown->collectable ) )
        {
          KEY_ACTION_TRACE_CREATE( SLOWDOWN_EXPIRED, 0 );

          /*
           * The return value of the timeout function is taken to indicate
           * whether the slowdown mode should be deactivated.
           */
          if( (*(slowdown->collectable.timer_fn))( &(slowdown->collectable), (void*)slowdown ) )
            *output_action = DEACTIVATE_SLOWDOWN;
          else
            *output_action = NO_ACTION;
        }

      }
      else if( !SLOWDOWNS_DISABLED )
      {
        /*
         * Pill is available and slowdowns are active. If he's walked onto it call the hander.
         */
        if( IS_COLLECTION_POINT( RUNNER_CENTRE_X(xpos),
                                 RUNNER_CENTRE_Y(ypos), slowdown->collectable ) )
        {
          KEY_ACTION_TRACE_CREATE( CONSUMED_SLOWDOWN, 0 );

          /*
           * The handler currently returns void. This could be changed to
           * return a flag to activate (or not) the slowdown. This is not
           * currently required.
           */
          (*(slowdown->collectable.collection_fn))( &(slowdown->collectable), (void*)slowdown );

          *output_action = ACTIVATE_SLOWDOWN;
          break;
        }
      }

      slowdown++;
    }
  }

  return KEEP_PROCESSING;
}




/***
 *       ____                     _____                 ___
 *      / __ \                   |  __ \               |__ \
 *     | |  | |_ __   ___ _ __   | |  | | ___   ___  _ __ ) |
 *     | |  | | '_ \ / _ | '_ \  | |  | |/ _ \ / _ \| '__/ /
 *     | |__| | |_) |  __| | | | | |__| | (_) | (_) | | |_|
 *      \____/| .__/ \___|_| |_| |_____/ \___/ \___/|_| (_)
 *            | |
 *            |_|
 */
PROCESSING_FLAG test_for_door_key( void* data, GAME_ACTION* output_action )
{
  GAME_STATE* game_state = (GAME_STATE*)data;

  *output_action = NO_ACTION;

  /* Skip everything if this level doesn't have doors */
  if( game_state->current_level->doors )
  {
    DOOR* door = game_state->current_level->doors;

    uint8_t xpos = GET_RUNNER_XPOS;
    uint8_t ypos = GET_RUNNER_YPOS;

    /* Loop over doors */
    while( IS_VALID_DOOR(door) )
    {
      /*
       * If the door has been opened and is active, reduce the
       * timer. If that's got to zero, reinstate the key.
       */
      if( !IS_COLLECTABLE_AVAILABLE( door->collectable ) )
      {
        DECREMENT_COLLECTABLE_TIMER( door->collectable );

        if( COLLECTABLE_TIMER_EXPIRED( door->collectable ) )
        {
          KEY_ACTION_TRACE_CREATE( SLOWDOWN_EXPIRED, 0 );

          /*
           * The return value of the timeout function is taken to indicate
           * whether the slowdown mode should be deactivated.
           */
          if( (*(door->collectable.timer_fn))( &door->collectable, (void*)door ) )
            *output_action = CLOSE_DOOR;
          else
            *output_action = NO_ACTION;
        }

      }
      else
      {
        /*
         * Key is available. If he's walked onto it call the hander.
         */
        if( IS_COLLECTION_POINT( RUNNER_CENTRE_X(xpos),
                                 RUNNER_CENTRE_Y(ypos), door->collectable ) )
        {
          KEY_ACTION_TRACE_CREATE( OPENED_DOOR, 0 );

          /*
           * The handler currently returns void. This could be changed to
           * return a flag to activate (or not) the door. This is not
           * currently required.
           */
          (*(door->collectable.collection_fn))( &(door->collectable), (void*)door );

          *output_action = OPEN_DOOR;
          break;
        }
      }

      door++;
    }
  }

  return KEEP_PROCESSING;
}




/***
 *                     _                 _             _                     ___
 *         /\         (_)               | |           | |                   |__ \
 *        /  \   _ __  _ _ __ ___   __ _| |_ ___    __| | ___   ___  _ __ ___  ) |
 *       / /\ \ | '_ \| | '_ ` _ \ / _` | __/ _ \  / _` |/ _ \ / _ \| '__/ __|/ /
 *      / ____ \| | | | | | | | | | (_| | ||  __/ | (_| | (_) | (_) | |  \__ |_|
 *     /_/    \_|_| |_|_|_| |_| |_|\__,_|\__\___|  \__,_|\___/ \___/|_|  |___(_)
 *
 *
 */
PROCESSING_FLAG animate_doors( void* data, GAME_ACTION* output_action )
{
  GAME_STATE* game_state = (GAME_STATE*)data;

  if( game_state->current_level->doors )
  {
    DOOR* door = game_state->current_level->doors;

    while( IS_VALID_COLLECTABLE(door->collectable) )
    {
      if( DOOR_IS_MOVING( door ) )
      {
        /*
         * This function is called 50 times a second, but I don't want to
         * animate the doors that fast. They just whizz away too quickly.
         * So keep a step count with the door and only animate every
         * few frames.
         */
        if( door->animation_step++ == 0 )
        {
          animate_door( door );
        }
        else
        {
          if( door->animation_step == 5 )
            door->animation_step = 0;
        }
      }
      else
      {
        /*
         * The door isn't moving, so check to see if it's open and he's reached it.
         * When a door opens its sprite moves aside. Once it's fully open the attribute
         * of the single cell the door occupies is set to the background which means
         * the collision code won't see the door and the runner will be able the
         * move through the doorway. Once that happens the door is set to stay
         * permanently open.
         * Check to see if he's reached this door. The function does what's necessary
         * to wedge the door open if he's reached it.
         */
        if( DOOR_IS_OPEN(door) )
        {
          check_door_passed_through( door );
        }
      }

      door++;
    }
  }

  *output_action = NO_ACTION;
  return KEEP_PROCESSING;
}

