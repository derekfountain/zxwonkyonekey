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

/*
 * The player's score for the level starts at a value defined in the level data.
 * Some levels are harder than others and will take longer, so levels starts at
 * different values. The level score reduces down to zero as time passes.
 *
 * When the player completes the level the remaining score is added onto the
 * total score.
 */

#include <stdint.h>
#include <stdlib.h>
#include <arch/zx.h>
#include <arch/zx/sp1.h>

#include "countdown.h"
#include "utils.h"

uint16_t game_countdown = 0;

/***
 *      _____  _           _                _____                  _      _
 *     |  __ \(_)         | |              / ____|                | |    | |
 *     | |  | |_ ___ _ __ | | __ _ _   _  | |     ___  _   _ _ __ | |_ __| | _____      ___ __
 *     | |  | | / __| '_ \| |/ _` | | | | | |    / _ \| | | | '_ \| __/ _` |/ _ \ \ /\ / | '_ \
 *     | |__| | \__ | |_) | | (_| | |_| | | |___| (_) | |_| | | | | || (_| | (_) \ V  V /| | | |
 *     |_____/|_|___| .__/|_|\__,_|\__, |  \_____\___/ \__,_|_| |_|\__\__,_|\___/ \_/\_/ |_| |_|
 *                  | |             __/ |
 *                  |_|            |___/
 */

static struct sp1_ss* slider_sprite;

uint8_t slider_colour = INK_BLUE;
static void initialise_colour(unsigned int count, struct sp1_cs *c)
{
  (void)count;    /* Suppress compiler warning about unused parameter */

  c->attr_mask = SP1_AMASK_INK;
  c->attr      = slider_colour;
}

#define SLIDER_PLANE 1

extern uint8_t score_slider[8];

void create_slider( void )
{
  slider_sprite = sp1_CreateSpr(SP1_DRAW_OR1LB, SP1_TYPE_1BYTE, 2, 0, SLIDER_PLANE);
  sp1_AddColSpr(slider_sprite, SP1_DRAW_OR1RB, SP1_TYPE_1BYTE, 0, SLIDER_PLANE);

  /* Colour the cells the sprite occupies */
  sp1_IterateSprChar(slider_sprite, initialise_colour);
}


#define SLIDER_WIDTH_IN_PIXELS ((uint8_t)(10*8))
#define STARTING_SCORE_OVER_SLIDER_WIDTH (/*STARTING_SCORE*/ 250/SLIDER_WIDTH_IN_PIXELS)

/*
 * This is defined in main.c. Just share it for now.
 * TODO Put it in a main.h
 */
extern struct sp1_Rect full_screen;

void update_countdown_slider( SCORE_SCREEN_DATA* screen_data )
{
  /*
   * Calculate:
   *   STARTING_SCORE/SLIDER_WIDTH_IN_PIXELS = POINTS_PER_PIXEL
   *   current_score /POINTS_PER_PIXEL       = PIXELS_FROM_ZERO
   *
   * eg
   *  10000/88  = 113
   *   9000/113 =  79 so 79 pixels from left side
   *
   *  50000/88  = 568
   *  45000/568 =  79
   */

  if( game_countdown )
  {
    uint16_t slider_x_pos = game_countdown / STARTING_SCORE_OVER_SLIDER_WIDTH;

    if( slider_x_pos < 8 )
    {
      slider_colour = INK_RED;

      /* Colour the cells the sprite occupies */
      sp1_IterateSprChar(slider_sprite, initialise_colour);
    }

    sp1_MoveSprPix(slider_sprite, &full_screen, (void*)score_slider,
                   ((screen_data->level_score_x)*8)+slider_x_pos, (screen_data->level_score_y)*8);
  }
}

