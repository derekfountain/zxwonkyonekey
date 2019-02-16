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

#include <stdint.h>
#include <arch/zx.h>
#include <arch/zx/sp1.h>

#include "slowdown_pill.h"
#include "countdown.h"
#include "bonus.h"

/*
 * Bonuses are linked to slowdown pills. This came about in a rather hacky
 * way as I was putting finishing touches to the game. You start with a
 * number of bonuses which are depleted each time to consume a slowdown
 * pill. When all the bonuses are gone all the slowdown pills disappear
 * and the game has to be completed at full speed.
 */

/*
 * The bonus sprites are never obscured so this isn't relevant really.
 */
#define BONUS_PLANE    (uint8_t)(1)

/* Number of bonuses still left */
uint8_t bonuses_left;

BONUS bonuses[STARTING_NUM_BONUSES];

/*
 * This is defined in main.c. Just share it for now.
 */
extern struct sp1_Rect full_screen;

/* This is in the assembly language file */
extern uint8_t bonus[];

static void initialise_colour(unsigned int count, struct sp1_cs *c)
{
  (void)count;    /* Suppress compiler warning about unused parameter */

  c->attr_mask = SP1_AMASK_INK;
  c->attr      = INK_RED;
}

void create_game_bonuses( uint8_t num_bonuses )
{
  uint8_t i;

  for( i=0; i<num_bonuses; i++ )
  {
    bonuses[i].sprite = sp1_CreateSpr(SP1_DRAW_LOAD1LB, SP1_TYPE_1BYTE, 2, 0, BONUS_PLANE);
    sp1_AddColSpr(bonuses[i].sprite, SP1_DRAW_LOAD1RB, SP1_TYPE_1BYTE, 0, BONUS_PLANE);

    /* Colour the cells the sprite occupies */
    sp1_IterateSprChar(bonuses[i].sprite, initialise_colour);
  }

  bonuses_left = num_bonuses;
}

void draw_bonuses( SCORE_SCREEN_DATA* screen_data )
{
  uint8_t x = screen_data->bonus_sprite0_x_pixel;
  uint8_t y = screen_data->bonus_sprite0_y_pixel;
  uint8_t i;

  /* Just line them up in the level-prescribed location */
  for( i=0; i<bonuses_left; i++ )
  {
    sp1_MoveSprPix(bonuses[i].sprite, &full_screen, (void*)bonus, x, y);
    x+=8;
  }
}

void lose_bonus( void )
{
  if( bonuses_left )
  {
    if( --bonuses_left == 0 )
      DISABLE_SLOWDOWNS;

    COUNTDOWN_APPLY_SLOWDOWN_PENALTY

    /*
     * I wanted the bonuses to fall off the screen, and wrote the code to do
     * it, but it slowed the game to less than 50fps. That was an initial
     * requirement, so I took the animation out again. This just moves the
     * sprite off screen. The bonuses can be converted to tiles if I need to
     * save a bit more space.
     */
    sp1_MoveSprPix(bonuses[bonuses_left].sprite, &full_screen, (void*)bonus, 255, 255);
  }
}

uint8_t query_bonuses_left( void )
{
  return bonuses_left;
}
