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
#include <stdint.h>
#include <z80.h>

#include "game_state.h"
#include "int.h"
#include "runner.h"
#include "action.h"
#include "key_action.h"
#include "levels.h"
#include "tracetable.h"
#include "gameloop.h"

struct sp1_Rect full_screen = {0, 0, 32, 24};

GAME_STATE game_state;

int main()
{
  if( is_rom_writable() ) {
    /* Flicker the border if ROM is being used for trace */
    zx_border(INK_RED);
    z80_delay_ms(100);
    zx_border(INK_BLUE);
    z80_delay_ms(100);
    zx_border(INK_WHITE);

    clear_trace_area();    
  }

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
