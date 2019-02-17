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
  uint8_t  countdown_slider_x;
  uint8_t  countdown_slider_y;
  uint8_t  score_screen_attribute;

  uint8_t  bonus_sprite0_x_pixel;
  uint8_t  bonus_sprite0_y_pixel;

} SCORE_SCREEN_DATA;

/*
 * Number of seconds the player starts with to complete the game.
 * Arrived at empirically, which is always a dubious thing to do,
 * given I've spent hours play testing it. :)
 *
 * With the emulator at 33% (which makes it much easier):
 *
 * Level 0 took 1:20
 * Level 1 took 1:25
 * Level 2 took 2:15
 * Level 3 took 1:35
 * Level 4 took 1:25
 *
 * That's 8 minutes, or 480 seconds. At full speed, 160 seconds.
 * 5 bonuses taken at 20 seconds penalty each, that's another
 * 100 seconds, so a perfect game, at full speed, with all bonuses
 * used, is going to require a starting point of 260 seconds.
 * That's going to be close to impossible, so add a completely
 * arbitrary 50%, gives 400 seconds. That's 6 minutes, 40 seconds,
 * or 5 minutes if the player doesn't take the bonuses.
 *
 * Reasonable? Heck knows... :)
 */
#define COUNTDOWN_START_SECS     ((uint16_t)400)

/*
 * Countdown secs lost when a slowdown pill is consumed.
 */
#define SLOWDOWN_PENALTY   ((uint16_t)20)

/*
 * Interface via macros for speed
 */
extern uint16_t game_countdown;
#define SET_GAME_COUNTDOWN(c)         (game_countdown=(c))
#define GET_GAME_COUNTDOWN            ((uint16_t)(game_countdown))
#define DECREMENT_GAME_COUNTDOWN      {if( game_countdown ) game_countdown--;}

/*
 * Be generous! Only subtract the slowdown consumption penalty if the
 * current countdown is still high enough not to go to zero.
 */
#define COUNTDOWN_APPLY_SLOWDOWN_PENALTY \
      if( game_countdown>SLOWDOWN_PENALTY ) game_countdown-=SLOWDOWN_PENALTY;\

void create_slider( void );
void reset_slider(void);
void update_countdown_slider( SCORE_SCREEN_DATA* );

#endif
