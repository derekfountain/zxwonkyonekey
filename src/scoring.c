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
 *
 * The level bonus starts at a value defined in the level data and is reduced
 * by a value defined in the level data each time the player uses a slowdown
 * block. This bit's a TODO.
 */

#include <stdint.h>
#include <stdlib.h>
#include <arch/zx/sp1.h>

#include "scoring.h"

uint32_t total_score = 0;

uint16_t level_score = 50000;
uint16_t level_bonus = 35000;

void set_level_score( uint16_t score )
{
  level_score = score;
}

uint16_t get_level_score( void )
{
  return level_score;
}

void decrement_level_score( uint16_t decrement )
{
  level_score -= decrement;
}

/***
 *      _____  _           _                _____
 *     |  __ \(_)         | |              / ____|
 *     | |  | |_ ___ _ __ | | __ _ _   _  | (___   ___ ___  _ __ ___ ___
 *     | |  | | / __| '_ \| |/ _` | | | |  \___ \ / __/ _ \| '__/ _ / __|
 *     | |__| | \__ | |_) | | (_| | |_| |  ____) | (_| (_) | | |  __\__ \
 *     |_____/|_|___| .__/|_|\__,_|\__, | |_____/ \___\___/|_|  \___|___/
 *                  | |             __/ |
 *                  |_|            |___/
 */

/*
 * This is defined in main.c. Just share it for now.
 */
extern struct sp1_Rect full_screen;

/*
 * TODO Need to remind myself why this should be static. Alvin told be in
 * a pull request comment. :)
 */
struct sp1_pss print_control = { &full_screen, SP1_PSSFLAG_INVALIDATE,
                                 0, 0,
                                 0x00, 0x03,
                                 0,
                                 0 };

void show_scores( SCORE_SCREEN_DATA* score_screen_data )
{
  /*
   * TODO This fails when the score gets down to less that 5 digits.
   * sprintf() would be too expensive, so this needs to work a bit
   * harder to get 0 padding and alignment.
   * I also need to cache the last displayed score and only update
   * the display when it changes. Doing this every loop is expensive.
   * Get the colours for the text into the level data too. Change colour
   * in the string below. I need to label the numbers, they look weird.
   *
   * Then add some sort of timer to reduce the score. Show the total score
   * between screens?
   */
  char print_string[15] = {0};

  print_string[0] = '\x16';
  print_string[1] = score_screen_data->level_score_y;
  print_string[2] = score_screen_data->level_score_x;
  utoa( level_score, &(print_string[3]), 10 );

  print_string[8] = '\x16';
  print_string[9] = score_screen_data->bonus_score_y;
  print_string[10] = score_screen_data->bonus_score_x;
  utoa( level_score, &(print_string[11]), 10 );

  sp1_PrintString(&print_control, (uint8_t*)print_string);
  
}

void display_level_score( void )
{

}

void display_bonus( void )
{

}

void display_total_score( void )
{

}

