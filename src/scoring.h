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

void     set_level_score( uint16_t score );
void     set_level_bonus( uint16_t bonus );
uint16_t get_level_score( void );
void     decrement_level_score( uint16_t decrement );

/*
 * Display routines. I'm not sure if Spectrum games benefit from separating out
 * the view code. Such fancy modern techniques seems to consume resources. :)
 * I'll keep this here for now.
 */

/*
 * Structure to hold the screen locations and colours etc. to place the scores.
 * These vary from level to level.
 */
typedef struct _score_screen_data
{
  uint8_t  level_score_x;
  uint8_t  level_score_y;

  uint8_t  bonus_score_x;
  uint8_t  bonus_score_y;

  uint8_t  score_screen_attribute;

} SCORE_SCREEN_DATA;

/*
 * Show scores on screen according to the given screen data.
 * Locations on screen, colours, etc., change from level to
 * level so the parameter is required to pass that in.
 */
void     show_scores( SCORE_SCREEN_DATA* score_screen_data );

/*
 * Scores are only updated on screen when they change because
 * it's an expensive operation which might feasibly have to be
 * done every cycle. This routine resets the caching, guaranteeing
 * the scores will be redrawn next cycle. This needs calling when
 * the level changes, probably among other key points.
 */
void     reset_cached_screen_scores( void );

void     display_total_score( void );

#endif
