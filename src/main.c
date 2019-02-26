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
#include "winner.h"
#include "bonus.h"
#include "countdown.h"

/* Hopefully the optimiser won't remove this. :) Keep it 8 bytes, BE expects that */
unsigned char version[8] = "ver1.01";

struct sp1_Rect full_screen = {0, 0, 32, 24};


GAME_STATE game_state;

void game_over( void )
{
  winner_banner();
  winner_fireworks();
}

void loser( void )
{
  loser_banner();
}

/*
 * Forward declare the level data. It's not external, it's at the bottom of this file,
 * but C insists it's declared extern.
 */
extern LEVEL_DATA level_data[];

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
    init_slowdown_trace();
    init_door_trace();
    init_collectable_trace();
  }

  setup_int();

  sp1_Initialize( SP1_IFLAG_MAKE_ROTTBL | SP1_IFLAG_OVERWRITE_TILES | SP1_IFLAG_OVERWRITE_DFILE,
                  INK_BLACK | PAPER_WHITE,
                  ' ' );

  setup_levels_font();

  create_runner();
  create_slider();
  create_game_bonuses( STARTING_NUM_BONUSES );

  /*
   * Outer loop loops over games. A winner game goes into infinite fireworks so this is a loop
   * over losing games.
   */
  while( 1 ) {
    reset_runner( RIGHT );
    reset_slider();
    reset_game_bonuses( STARTING_NUM_BONUSES );

    SET_GAME_COUNTDOWN( 0 );

    current_level_num = 0;
    while( 1 ) {
      LEVEL_COMPLETION_TYPE completion_type;

      /* Get the level data and call it's draw function to draw it */
      game_state.current_level = &level_data[current_level_num];
      print_level_from_sp1_string( game_state.current_level );

      sp1_Invalidate(&full_screen);
      sp1_UpdateNow();

      /* Wait in case the user is holding down the control key */
      while( in_key_pressed( IN_KEY_SCANCODE_SPACE ) );
      game_state.key_pressed = 0;
      game_state.key_processed = 0;

      /* Runner at start point */
      zx_border( game_state.current_level->border_colour );
      SET_RUNNER_FACING( game_state.current_level->start_facing );
      SET_RUNNER_XPOS( game_state.current_level->start_x );
      SET_RUNNER_YPOS( game_state.current_level->start_y );
      set_runner_colour( game_state.current_level->background_att );
      SET_RUNNER_SLOWDOWN( SLOWDOWN_INACTIVE );

      /* Enter game loop, exit when player completes the level */
      completion_type = gameloop( &game_state );

      /* Call the level's teardown function to reclaim resources */
      teardown_level( game_state.current_level );

      /* Did the level end because time was up? If so break to outer loop to reset everything */
      if( completion_type == GAME_COMPLETE_LOSER ) {
        loser();
        break;
      }

      if( ++current_level_num == NUM_LEVELS ) {
        game_over();
      }

      /*
       * Another crude hack caused by having level 0 as the introduction.
       * The countdown can't start at level 0 - that's hardly fair. So
       * it gets started here when the proper 1st level starts.
       */
      if( current_level_num == 1 ) {
        SET_GAME_COUNTDOWN( COUNTDOWN_START_SECS );
      }
    }
  }
}
