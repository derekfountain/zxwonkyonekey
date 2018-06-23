#include <arch/zx.h>
#include <stdio.h>
#include "utils.h"
#include "action.h"
#include "game_state.h"

GAME_ACTION test_for_direction_change( void* data )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address;
  uint8_t     attr;

  if( MODULO8(game_state->runner->xpos) == 0 ) {
    if( game_state->runner->facing == RIGHT )
      attr_address = zx_pxy2aaddr( game_state->runner->xpos+8, game_state->runner->ypos );
    else
      attr_address = zx_pxy2aaddr( game_state->runner->xpos-8, game_state->runner->ypos );
    attr = *attr_address;
  
    if( (attr & 0x38) != PAPER_WHITE )
      return TOGGLE_DIRECTION;
  }

  if( game_state->key_pressed && ! game_state->key_processed ) {
    game_state->key_processed = 1;
    return TOGGLE_DIRECTION;
  }

  return NO_ACTION;
}

GAME_ACTION test_for_start_jump( void* data )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address;
  uint8_t     attr;

  /* Are we already jumping? If so, no action */
  if( game_state->runner->jump_offset != NOT_JUMPING )
    return NO_ACTION;

  /* Are we on a trampoline block? If not, no action */
  attr_address = zx_pxy2aaddr( game_state->runner->xpos, game_state->runner->ypos+8  );
  attr = *attr_address;

  if( (attr & 0x38) != PAPER_RED ) {
    if( MODULO8(game_state->runner->xpos) == 0 )
      return NO_ACTION;
      
    attr_address = zx_pxy2aaddr( game_state->runner->xpos+8, game_state->runner->ypos+8  );
    attr = *attr_address;

    if( (attr & 0x38) != PAPER_RED )
      return NO_ACTION;
  }

  if( game_state->key_pressed && ! game_state->key_processed ) {
    game_state->key_processed = 1;
    return JUMP;
  }

  return NO_ACTION;
}

GAME_ACTION test_for_falling( void* data )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address;
  uint8_t     attr;

  /* Are we in the middle of a jump? If so, no action */
  if( game_state->runner->jump_offset != NOT_JUMPING )
    return NO_ACTION;

  /* Are we on a coloured block? If not, no action */
  attr_address = zx_pxy2aaddr( game_state->runner->xpos, game_state->runner->ypos+8  );
  attr = *attr_address;
  
  if( (attr & 0x38) != PAPER_WHITE )
    return NO_ACTION;

  /* So player is on a clear block. If aligned to the cell, fall through */
  if( MODULO8(game_state->runner->xpos) == 0 )
    return MOVE_DOWN;

  return NO_ACTION;
}
