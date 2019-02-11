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

#include "bonus.h"

/*
 * The bonus sprites sit some way back. Everything else will cover them.
 */
#define BONUS_PLANE    (uint8_t)(25)

/* Number of bonuses still left */
uint8_t bonuses_left;

BONUS bonuses[STARTING_NUM_BONUSES];

/*
 * This is defined in main.c. Just share it for now.
 */
extern struct sp1_Rect full_screen;

/* This is in the assembly language file */
extern uint8_t apple[];

static uint8_t ink_param;
static void initialise_colour(unsigned int count, struct sp1_cs *c)
{
  (void)count;    /* Suppress compiler warning about unused parameter */

  c->attr_mask = SP1_AMASK_INK;
  c->attr      = ink_param;
}

void create_game_bonuses( uint8_t num_bonuses )
{
  uint8_t i;

  for( i=0; i<num_bonuses; i++ )
  {
    bonuses[i].sprite = sp1_CreateSpr(SP1_DRAW_LOAD1LB, SP1_TYPE_1BYTE, 2, 0, BONUS_PLANE);
    sp1_AddColSpr(bonuses[i].sprite, SP1_DRAW_LOAD1RB, SP1_TYPE_1BYTE, 0, BONUS_PLANE);

    /* Colour the cells the sprite occupies */
    ink_param = INK_GREEN;
    sp1_IterateSprChar(bonuses[i].sprite, initialise_colour);
  }

  bonuses_left = num_bonuses;
}

void draw_bonuses( SCORE_SCREEN_DATA* screen_data )
{
  register uint8_t x = screen_data->bonus_sprite0_x_pixel;
  register uint8_t y = screen_data->bonus_sprite0_y_pixel;
  uint8_t i;

  for( i=0; i<bonuses_left; i++ )
  {
    sp1_MoveSprPix(bonuses[i].sprite, &full_screen,
                  (void*)apple,
                  x, y);
    x+=8;
  }
}

void lose_bonus( void )
{
  if( bonuses_left )
  {
    sp1_MoveSprPix(bonuses[bonuses_left-1].sprite, &full_screen,
                  (void*)apple,
                  255, 255);

    bonuses_left--;
  }
}
