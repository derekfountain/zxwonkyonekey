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

#define GAMELOOP_TRACE_ENTRIES 500
#define GAMELOOP_TRACETABLE_SIZE ((size_t)sizeof(GAMELOOP_TRACE)*GAMELOOP_TRACE_ENTRIES)

GAMELOOP_TRACE* gameloop_tracetable = TRACING_UNINITIALISED;
GAMELOOP_TRACE* gameloop_next_trace = 0xFFFF;

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

void gameloop_add_trace( GAMELOOP_TRACE* glt_ptr )
{
  memcpy(gameloop_next_trace, glt_ptr, sizeof(GAMELOOP_TRACE));

  gameloop_next_trace = (void*)((uint8_t*)gameloop_next_trace + sizeof(GAMELOOP_TRACE));

  if( gameloop_next_trace == (void*)((uint8_t*)gameloop_tracetable+GAMELOOP_TRACETABLE_SIZE) )
      gameloop_next_trace = gameloop_tracetable;
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
    {test_for_finish                },
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
  if( gameloop_tracetable == TRACING_UNINITIALISED ) {
    gameloop_tracetable = gameloop_next_trace = allocate_tracememory(GAMELOOP_TRACETABLE_SIZE);
  }

  while(1) {
    uint8_t     i;

    if( in_key_pressed( IN_KEY_SCANCODE_SPACE ) ) {
      game_state->key_pressed = 1;
    } else {
      game_state->key_pressed = 0;
      game_state->key_processed = 0;
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

    intrinsic_halt();
    sp1_UpdateNow();
  }
}
