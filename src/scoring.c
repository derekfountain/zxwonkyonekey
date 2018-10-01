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

uint32_t total_score;

uint16_t level_score;
uint16_t level_bonus;

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

void display_level_score( void )
{

}

void display_bonus( void )
{

}

void display_total_score( void )
{

}

