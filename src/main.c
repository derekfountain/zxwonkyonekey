#include <arch/zx.h>
#include <arch/zx/sp1.h>
#include <intrinsic.h>
#include <input.h>
#include <stdint.h>
#include <z80.h>

#include "game_state.h"
#include "int.h"
#include "runner.h"
#include "action.h"
#include "key_action.h"
#include "levels.h"
#include "tracetable.h"

struct sp1_Rect full_screen = {0, 0, 32, 24};

GAME_STATE game_state;

LOOP_ACTION game_actions[] =
  {
    {test_for_falling,          &game_state},
    {test_for_start_jump,       &game_state},
    {test_for_direction_change, &game_state},
    {move_sideways,             &game_state},
  };
#define NUM_GAME_ACTIONS (sizeof(game_actions) / sizeof(LOOP_ACTION))

int main()
{
  if( is_rom_writable() ) {
    /* Flicker the border if ROM is being used for trace */

    zx_border(INK_RED);
    z80_delay_ms(100);
    zx_border(INK_BLUE);
    z80_delay_ms(100);
    zx_border(INK_WHITE);
  }

  zx_border(INK_BLACK);
  setup_int();

  sp1_Initialize( SP1_IFLAG_MAKE_ROTTBL | SP1_IFLAG_OVERWRITE_TILES | SP1_IFLAG_OVERWRITE_DFILE,
                  INK_BLACK | PAPER_WHITE,
                  ' ' );

  game_state.runner = create_runner( RIGHT );

  level1();
  sp1_Invalidate(&full_screen);
  sp1_UpdateNow();

  /* Wait in case the user is holding down the control key */
  while( in_key_pressed( IN_KEY_SCANCODE_SPACE ) );
  game_state.key_pressed = 0;
  game_state.key_processed = 0;

  game_state.runner->xpos = 0;
  game_state.runner->ypos = 96;

  while(1) {
    uint8_t i;
    GAME_ACTION action;

    if( in_key_pressed( IN_KEY_SCANCODE_SPACE ) ) {
      game_state.key_pressed = 1;
    } else {
      game_state.key_pressed = 0;
      game_state.key_processed = 0;
    }

    for( i=0; i < NUM_GAME_ACTIONS; i++ ) {
      action = (game_actions[i].test_action)(game_actions[i].data);
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
      game_state.runner->ypos++;
      break;

    case MOVE_UP:
      game_state.runner->ypos--;
      break;

    case MOVE_RIGHT:
      game_state.runner->xpos++;
      break;

    case MOVE_LEFT:
      game_state.runner->xpos--;
      break;

    }

    position_runner( game_state.runner->xpos, &game_state.runner->ypos );

    sp1_UpdateNow();
    intrinsic_halt();
//    z80_delay_ms(25);
  }
}
