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

#include <stdio.h>

typedef enum _gameloop_tracetype
{
  ENTER,
  KEY_STATE,
  ACTION,
  EXIT,
} GAMELOOP_TRACETYPE;

typedef struct _gameloop_trace
{
  GAMELOOP_TRACETYPE tracetype;
  uint8_t            key_pressed;
  uint8_t            key_processed;
  GAME_ACTION        action;
} GAMELOOP_TRACE;

#define GAMELOOP_TRACE_ENTRIES 50
#define GAMELOOP_TRACETABLE_SIZE ((size_t)sizeof(GAMELOOP_TRACE)*GAMELOOP_TRACE_ENTRIES)

GAMELOOP_TRACE* gameloop_tracetable = 0xFFFF;
GAMELOOP_TRACE* gameloop_next_trace = 0xFFFF;

void gameloop_add_trace( GAMELOOP_TRACETYPE type, uint8_t kpressed, uint8_t kprocessed, GAME_ACTION act )
{
  GAMELOOP_TRACE glt;

  /* TODO This will be faster to load the fields in the caller and pass a pointer to the struct? */
  glt.tracetype =     type; 
  glt.key_pressed =   kpressed;
  glt.key_processed = kprocessed;
  glt.action =        act;
  memcpy(gameloop_next_trace, &glt, sizeof(glt));

  gameloop_next_trace = (void*)((uint8_t*)gameloop_next_trace + sizeof(glt));

  if( gameloop_next_trace == (void*)((uint8_t*)gameloop_tracetable+GAMELOOP_TRACETABLE_SIZE) )
      gameloop_next_trace = gameloop_tracetable;
}

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
  if( gameloop_tracetable == (GAMELOOP_TRACE*)0xFFFF )
    gameloop_tracetable = gameloop_next_trace = allocate_tracetable(GAMELOOP_TRACETABLE_SIZE);

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

    gameloop_add_trace(ACTION, game_state->key_pressed, game_state->key_processed, action);

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
