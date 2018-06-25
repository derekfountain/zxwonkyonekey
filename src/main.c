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
#include "gameloop.h"

struct sp1_Rect full_screen = {0, 0, 32, 24};

GAME_STATE game_state;

int main()
{
  zx_border(INK_BLACK);
  setup_int();

  sp1_Initialize( SP1_IFLAG_MAKE_ROTTBL | SP1_IFLAG_OVERWRITE_TILES | SP1_IFLAG_OVERWRITE_DFILE,
                  INK_BLACK | PAPER_WHITE,
                  ' ' );

  game_state.runner = create_runner( RIGHT );

  while( 1 ) {

    /* Draw the level */
    level1();
    sp1_Invalidate(&full_screen);
    sp1_UpdateNow();

    /* Wait in case the user is holding down the control key */
    while( in_key_pressed( IN_KEY_SCANCODE_SPACE ) );
    game_state.key_pressed = 0;
    game_state.key_processed = 0;

    /* Runner at start point */
    game_state.runner->xpos = 0;
    game_state.runner->ypos = 96;

    /* Enter game loop, exit when player dies */
    gameloop( &game_state );
  }
}
