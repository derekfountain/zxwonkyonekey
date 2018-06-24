#include <arch/zx.h>
#include <arch/zx/sp1.h>
#include <intrinsic.h>
#include <input.h>
#include <stdint.h>
#include <z80.h>

#include "game_state.h"
#include "runner.h"
#include "action.h"
#include "key_action.h"
#include "levels.h"

LOOP_ACTION game_actions[] =
  {
    {test_for_falling               },
    {test_for_start_jump            },
    {test_for_direction_change      },
    {move_sideways                  },
  };
#define NUM_GAME_ACTIONS (sizeof(game_actions) / sizeof(LOOP_ACTION))

void gameloop( GAME_STATE* game_state )
{
  while(1) {
    uint8_t     i;
    GAME_ACTION action;

    if( in_key_pressed( IN_KEY_SCANCODE_SPACE ) ) {
      game_state->key_pressed = 1;
    } else {
      game_state->key_pressed = 0;
      game_state->key_processed = 0;
    }

    for( i=0; i < NUM_GAME_ACTIONS; i++ ) {
      action = (game_actions[i].test_action)(game_state);
      if( action != NO_ACTION )
	break;
    }

    switch( action )
    {
    case TOGGLE_DIRECTION:
      toggle_runner_direction();
      break;

    case JUMP:
      start_runner_jumping();
      break;

    case MOVE_DOWN:
      game_state->runner->ypos++;
      break;

    case MOVE_UP:
      game_state->runner->ypos--;
      break;

    case MOVE_RIGHT:
      game_state->runner->xpos++;
      break;

    case MOVE_LEFT:
      game_state->runner->xpos--;
      break;

    }

    position_runner( game_state->runner->xpos, &game_state->runner->ypos );

    sp1_UpdateNow();
    intrinsic_halt();
  }
}
