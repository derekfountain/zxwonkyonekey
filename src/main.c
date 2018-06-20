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

struct sp1_Rect full_screen = {0, 0, 32, 24};

GAME_STATE game_state;

LOOP_ACTION game_actions[] =
  {
    {test_for_keypress, &game_state},
    {NULL, NULL}
  };

int main()
{
  zx_border(INK_BLACK);
  setup_int();

  sp1_Initialize( SP1_IFLAG_MAKE_ROTTBL | SP1_IFLAG_OVERWRITE_TILES | SP1_IFLAG_OVERWRITE_DFILE,
                  INK_BLACK | PAPER_WHITE,
                  ' ' );

  create_runner_sprite( RUNNER_RIGHT );

  level1();

  sp1_Invalidate(&full_screen);
 
  while( in_key_pressed( IN_KEY_SCANCODE_SPACE ) );
  game_state.key_pressed = 0;
  game_state.key_processed = 0;

  game_state.player_xpos = 0;
  game_state.player_ypos = 80;

  while(1) {
    uint8_t i;
    GAME_ACTION action;

    if( in_key_pressed( IN_KEY_SCANCODE_SPACE ) ) {
      game_state.key_pressed = 1;
    } else {
      game_state.key_pressed = 0;
      game_state.key_processed = 0;
    }

    for( i=0; i < 1; i++ ) {
      action = (game_actions[i].test_action)(game_actions[i].data);
    }

    if( action == CHANGE_DIRECTION )
      change_runner_direction();

    position_runner( &game_state.player_xpos, &game_state.player_ypos );

    sp1_UpdateNow();
    intrinsic_halt();
    z80_delay_ms(25);
  }
}
