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

#ifndef __SCORING_H
#define __SCORING_H

#include <stdint.h>

/*
 * Structure to hold the screen locations and colours etc. to place the scores.
 * These vary from level to level.
 */
typedef struct _score_screen_data
{
  uint8_t  level_score_x;
  uint8_t  level_score_y;
  uint8_t  score_screen_attribute;

  uint8_t  bonus_sprite0_x_pixel;
  uint8_t  bonus_sprite0_y_pixel;

} SCORE_SCREEN_DATA;

/*
 * Interface via macros for speed
 */
extern uint16_t game_countdown;
#define SET_GAME_COUNTDOWN(c)       (game_countdown=(c))
#define GET_GAME_COUNTDOWN          ((uint16_t)(game_countdown))
#define DECREMENT_GAME_COUNTDOWN    {if( game_countdown ) game_countdown--;}

void     create_slider( void );

#endif
