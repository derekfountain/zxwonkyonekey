#include "action.h"
#include "game_state.h"

GAME_ACTION test_for_keypress( void* data )
{
  GAME_STATE* game_state = (GAME_STATE*)data;

  if( game_state->key_pressed && ! game_state->key_processed ) {
    game_state->key_processed = 1;
    return TOGGLE_DIRECTION;
  }

  return NO_ACTION;
}
