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
#include "collision.h"

struct sp1_Rect full_screen = {0, 0, 32, 24};

GAME_STATE game_state;

void game_over( void )
{
  /* TODO... */
  zx_border( INK_GREEN );
  while(1);
}

int main()
{
  uint8_t current_level_num;

  if( is_rom_writable() ) {
    /* Flicker the border if ROM is being used for trace */
    zx_border(INK_RED);
    z80_delay_ms(100);
    zx_border(INK_BLUE);
    z80_delay_ms(100);
    zx_border(INK_WHITE);

    clear_trace_area();    

    init_gameloop_trace();
    init_key_action_trace();
    init_collision_trace();
    init_runner_trace();
    init_door_trace();
  }

  setup_int();

  sp1_Initialize( SP1_IFLAG_MAKE_ROTTBL | SP1_IFLAG_OVERWRITE_TILES | SP1_IFLAG_OVERWRITE_DFILE,
                  INK_BLACK | PAPER_WHITE,
                  ' ' );


  create_runner( RIGHT );

  current_level_num = 3;
  while( 1 ) {
    
    /* Get the level data and call it's draw function to draw it */
    game_state.current_level = get_level_data( current_level_num );
    (game_state.current_level->draw_func)(game_state.current_level);

    sp1_Invalidate(&full_screen);
    sp1_UpdateNow();

    /* Wait in case the user is holding down the control key */
    while( in_key_pressed( IN_KEY_SCANCODE_SPACE ) );
    game_state.key_pressed = 0;
    game_state.key_processed = 0;

    /* Runner at start point */
    zx_border( game_state.current_level->border_colour );
    set_runner_facing( game_state.current_level->start_facing );
    set_runner_xpos( game_state.current_level->start_x );
    set_runner_ypos( game_state.current_level->start_y );
    set_runner_colour( game_state.current_level->background_att );
    set_runner_slowdown( SLOWDOWN_INACTIVE );

    set_level_score( game_state.current_level->max_score );
    set_level_bonus( game_state.current_level->max_bonus );
    reset_cached_screen_scores();

    /* Enter game loop, exit when player completes the level */
    gameloop( &game_state );

    /* Call the level's teardown function to reclaim resources */
    (game_state.current_level->teardown_func)(game_state.current_level);

    if( ++current_level_num == NUM_LEVELS ) {
      game_over();
      current_level_num = 0;
    }
  }
}
