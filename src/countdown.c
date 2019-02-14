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

uint32_t total_score = 0;

uint16_t level_score = 5000;
uint16_t last_printed_level_score = 0;

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
  if( level_score )
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

static struct sp1_ss* slider_sprite;

static void initialise_colour(unsigned int count, struct sp1_cs *c)
{
  (void)count;    /* Suppress compiler warning about unused parameter */

  c->attr_mask = SP1_AMASK_INK;
  c->attr      = INK_WHITE;
}

extern uint8_t score_slider[8];

#define SLIDER_PLANE 1

void create_slider( void )
{
  slider_sprite = sp1_CreateSpr(SP1_DRAW_LOAD1LB, SP1_TYPE_1BYTE, 2, 0, SLIDER_PLANE);
  sp1_AddColSpr(slider_sprite, SP1_DRAW_LOAD1RB, SP1_TYPE_1BYTE, 0, SLIDER_PLANE);

  /* Colour the cells the sprite occupies */
  sp1_IterateSprChar(slider_sprite, initialise_colour);
}

/*
 * This is in the levels code. It can only be used after the levels code
 * has initialised it.
 */
extern struct sp1_pss level_print_control;

static uint8_t slider_sp1_string[16] = "\x16" "yx" "\x8c" "\x8e\x8e\x8e\x8e\x8e\x8e\x8e\x8e\x8e" "\x8d\0";
void initialise_score_tiles( uint8_t x_cell, uint8_t y_cell )
{
  /* Build and print the string to display the slider */
  slider_sp1_string[1] = y_cell;
  slider_sp1_string[2] = x_cell;

  sp1_PrintString(&level_print_control, slider_sp1_string);
}
