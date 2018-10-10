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
#include <arch/zx/sp1.h>
#include <intrinsic.h>
#include <input.h>
#include <string.h>
#include <stdint.h>
#include <z80.h>

#include "game_state.h"
#include "runner.h"
#include "action.h"
#include "key_action.h"
#include "levels.h"
#include "tracetable.h"
#include "local_assert.h"
#include "int.h"
#include "collision.h"
#include "scoring.h"


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
 * This defines the game loop's trace table.
 */

typedef enum _gameloop_tracetype
{
  ENTER,
  KEY_STATE,
  ACTION,
  INT_100MS,
  INT_500MS,
  EXIT,
} GAMELOOP_TRACETYPE;

typedef struct _gameloop_trace
{
  uint16_t           ticker;
  GAMELOOP_TRACETYPE tracetype;
  uint8_t            key_pressed;
  uint8_t            key_processed;
  uint8_t            xpos;
  uint8_t            ypos;
  GAME_ACTION        action;
  PROCESSING_FLAG    processing_flag;
} GAMELOOP_TRACE;

/* BE:PICKUPDEF */
#define GAMELOOP_TRACE_ENTRIES 500
#define GAMELOOP_TRACETABLE_SIZE ((size_t)sizeof(GAMELOOP_TRACE)*GAMELOOP_TRACE_ENTRIES)

/* It's quicker to do this with a macro, as long as it's only used once or twice */
#define GAMELOOP_TRACE_CREATE(ttype,keypressed,keyprocessed,x,y,act,pflag) { \
    if( gameloop_tracetable != TRACING_INACTIVE ) { \
      GAMELOOP_TRACE      glt;   \
      glt.ticker          = GET_TICKER; \
      glt.tracetype       = ttype; \
      glt.key_pressed     = keypressed; \
      glt.key_processed   = keyprocessed; \
      glt.xpos            = x; \
      glt.ypos            = y; \
      glt.action          = act; \
      glt.processing_flag = pflag; \
      gameloop_add_trace(&glt); \
    } \
}

TRACE_FN( gameloop, GAMELOOP_TRACE, GAMELOOP_TRACETABLE_SIZE )

void init_gameloop_trace(void)
{
  if( gameloop_tracetable == TRACING_UNINITIALISED )
    gameloop_tracetable = gameloop_next_trace = allocate_tracememory(GAMELOOP_TRACETABLE_SIZE);
}


PROCESSING_FLAG service_interrupt_100ms( void* data, GAME_ACTION* output_action )
{
  (void)data;

  if( interrupt_service_required_100ms )
  {
      GAMELOOP_TRACE_CREATE(INT_100MS, 0, 0, 0, 0, 0, 0);

      decrement_level_score( 1 );

      /*
       * This flag is 8 bit and the compiler doesn't promote it, so it doesn't
       * need atomic protection.
       */
      interrupt_service_required_100ms = 0;
  }

  *output_action = NO_ACTION;
  return KEEP_PROCESSING;
}

PROCESSING_FLAG service_interrupt_500ms( void* data, GAME_ACTION* output_action )
{
  if( interrupt_service_required_500ms )
  {
    GAME_STATE* game_state = (GAME_STATE*)data;
    SLOWDOWN_DEFINITION* slowdown = game_state->current_level->slowdowns;

    while( slowdown->x || slowdown->y )
    {
      animate_slowdown_pill( slowdown );
      slowdown++;
    }
      
    GAMELOOP_TRACE_CREATE(INT_500MS, 0, 0, 0, 0, 0, 0);

    /*
     * This flag is 8 bit and the compiler doesn't promote it, so it doesn't
     * need atomic protection.
     */
    interrupt_service_required_500ms = 0;
  }

  *output_action = NO_ACTION;
  return KEEP_PROCESSING;
}

/***
 *       _____                                     _   _                 
 *      / ____|                          /\       | | (_)                
 *     | |  __  __ _ _ __ ___   ___     /  \   ___| |_ _  ___  _ __  ___ 
 *     | | |_ |/ _` | '_ ` _ \ / _ \   / /\ \ / __| __| |/ _ \| '_ \/ __|
 *     | |__| | (_| | | | | | |  __/  / ____ | (__| |_| | (_) | | | \__ \
 *      \_____|\__,_|_| |_| |_|\___| /_/    \_\___|\__|_|\___/|_| |_|___/
 *                                                                       
 *                                                                       
 * Game actions are the actions which happen every game loop. This is a carefully
 * ordered list of functions, called each time round the loop. When one returns
 * a value which isn't NO_ACTION the list processing stops and what was returned
 * is the action to take for this time round the game loop.
 *
 * This loop starts as the player has just been moved and respawned in his new
 * place, so the first checks should be to see if he's moved onto a gap to fall
 * through, etc.
 */

LOOP_ACTION game_actions[] =
  {
    {service_interrupt_100ms        },
    {service_interrupt_500ms        },
    {test_for_finish                },
    {test_for_teleporter            },
    {test_for_falling               },
    {test_for_start_jump            },
    {test_for_direction_change      },
    {act_on_collision               },
    {adjust_for_jump                },
    {move_sideways                  },
  };
#define NUM_GAME_ACTIONS (sizeof(game_actions) / sizeof(LOOP_ACTION))


void finish_level(void)
{
  /* Do I need anything here? Trace point, maybe? */
}

/***
 *      _______ _             _____                        _                       
 *     |__   __| |           / ____|                      | |                      
 *        | |  | |__   ___  | |  __  __ _ _ __ ___   ___  | |     ___   ___  _ __  
 *        | |  | '_ \ / _ \ | | |_ |/ _` | '_ ` _ \ / _ \ | |    / _ \ / _ \| '_ \ 
 *        | |  | | | |  __/ | |__| | (_| | | | | | |  __/ | |___| (_) | (_) | |_) |
 *        |_|  |_| |_|\___|  \_____|\__,_|_| |_| |_|\___| |______\___/ \___/| .__/ 
 *                                                                          | |    
 *                                                                          |_|    
 */
void gameloop( GAME_STATE* game_state )
{

  while(1) {
    uint8_t     i;

    if( in_key_pressed( IN_KEY_SCANCODE_SPACE ) ) {
      game_state->key_pressed = 1;
    } else {
      game_state->key_pressed = 0;
      game_state->key_processed = 0;
    }

    if( in_key_pressed( IN_KEY_SCANCODE_q ) ) {
      finish_level();
      break;
    }

    for( i=0; i < NUM_GAME_ACTIONS; i++ ) {
      PROCESSING_FLAG flag;
      GAME_ACTION     required_action;

      flag = (game_actions[i].test_action)(game_state, &required_action);

      if( required_action != NO_ACTION ) {
	GAMELOOP_TRACE_CREATE(ACTION, game_state->key_pressed,
			              game_state->key_processed,
                                      get_runner_xpos(),
                                      get_runner_ypos(),
                                      required_action,
                                      flag);
      }

      switch( required_action )
        {
        case TOGGLE_DIRECTION:
          toggle_runner_direction();
          break;

        case JUMP:
          start_runner_jumping();
          break;

        case STOP_JUMP:
          stop_runner_jumping();
          break;

        case MOVE_DOWN:
          move_runner_ypos(1);
          break;

        case MOVE_UP:
          move_runner_ypos(-1);
          break;

        case MOVE_RIGHT:
          move_runner_xpos(1);
          break;

        case MOVE_LEFT:
          move_runner_xpos(-1);
          break;

        case FINISH:
          finish_level();
          return;
        }

      if( flag == STOP_PROCESSING )
	break;
    }

    draw_runner();
    show_scores( &(game_state->current_level->score_screen_data) );
    
    /*
     * If the level has teleporters the cells they occupy are validated so
     * the runner doesn't get redrawn when he moves "onto" one. The effect
     * is for him to move "into" it which is what I want.
     *
     * This invalidation has to be done here each time round the loop because
     * when the runner sprite gets close the process of drawing him invalidates
     * the cell the teleporter is on, so the cell gets redrawn as runner sprite
     * but no teleporter. Specifically revalidating the teleporters ensures
     * they always appear even at the "cost" of not drawing the runner.
     */
    if( game_state->current_level->teleporters )
    {
      TELEPORTER_DEFINITION* teleporter = game_state->current_level->teleporters;

      while( teleporter->end_1_x || teleporter->end_1_y ) {
        /*
         * There doesn't appear to be an SP1 interface to validate a
         * single tile. Use a 1x1 rectangle instead.
         */
        struct sp1_Rect validate_cell = {teleporter->end_1_y_cell, teleporter->end_1_x_cell, 1, 1};
        sp1_Validate(&validate_cell);

        validate_cell.row = teleporter->end_2_y_cell;
        validate_cell.col = teleporter->end_2_x_cell;
        sp1_Validate(&validate_cell);

        teleporter++;
      }
    }

    /* Check if he's walked onto a slowdown pill */
    if( game_state->current_level->slowdowns )
    {
      SLOWDOWN_DEFINITION* slowdown = game_state->current_level->slowdowns;

      while( slowdown->x || slowdown->y )
      {
        /* This doesn't look right - surely this should be in a game action? */
        slowdown++;
      }
    }

    /* Halt to lock the game to 50fps, then update everything */
    intrinsic_halt();
    sp1_UpdateNow();
  }
}
