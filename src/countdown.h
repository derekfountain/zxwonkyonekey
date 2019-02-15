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
 * TODO How do I calculate this?
 */
#define STARTING_SCORE     ((uint16_t)10000)

/*
 * Countdown amount lost when a slowdown pill is consumed.
 */
/* TODO Calculate this, somehow */
#define SLOWDOWN_PENALTY   ((uint16_t)500)

/*
 * Interface via macros for speed
 */
extern uint16_t game_countdown;
#define SET_GAME_COUNTDOWN(c)         (game_countdown=(c))
#define GET_GAME_COUNTDOWN            ((uint16_t)(game_countdown))
#define DECREMENT_GAME_COUNTDOWN      {if( game_countdown ) game_countdown--;}

/*
 * Be generous! Only subract the slowdown consumption penalty if the
 * current countdown is still high enough not to go to zero.
 */
#define COUNTDOWN_APPLY_SLOWDOWN_PENALTY \
      if( game_countdown>SLOWDOWN_PENALTY ) game_countdown-=SLOWDOWN_PENALTY;\

void create_slider( void );

#define SLIDER_WIDTH_IN_PIXELS ((uint8_t)(11*8))

/*
 * TODO I think this needs to calculate 
 *   STARTING_SCORE/SLIDER_WIDTH_IN_PIXELS = POINTS_PER_PIXEL
 *   current_score /POINTS_PER_PIXEL       = PIXELS_FROM_ZERO
 *
 * eg
 *  10000/88  = 113
 *   9000/113 =  79 so 79 pixels from zero
 *
 *  50000/88  = 568
 *  45000/568 =  79
 */
void position_slider( uint16_t current_countdown );

#endif
