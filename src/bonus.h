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

#ifndef __BONUS_H
#define __BONUS_H

#include <arch/zx/sp1.h>

#include "countdown.h"

typedef struct _bonus
{
  /* SP1 sprite bonus is shown on screen as */
  struct sp1_ss*     sprite;
} BONUS;

#define STARTING_NUM_BONUSES  5

void create_game_bonuses( uint8_t num_bonuses );
void reset_game_bonuses( uint8_t num_bonuses );
void draw_bonuses( SCORE_SCREEN_DATA* screen_data );
void lose_bonus( void );
uint8_t query_bonuses_left( void );

#endif
