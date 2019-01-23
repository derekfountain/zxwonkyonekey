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

#ifndef __LEVELS_H
#define __LEVELS_H

#include <stdint.h>

#include "runner.h"
#include "utils.h"
#include "scoring.h"
#include "teleporter.h"
#include "door.h"
#include "slowdown_pill.h"

/*
 * Tile definition is essentially a user defined graphic for the sp1 library.
 * Tile numbers for UDGs start at 128.
 */
typedef struct _tile_definition
{
  uint8_t  tile_num;
  uint8_t* udg_data;
} TILE_DEFINITION;


typedef struct _level_data
{
  void*     draw_data;

  uint8_t   start_x;
  uint8_t   start_y;

  uint8_t   border_colour;

  DIRECTION start_facing;

  /*
   * Background attribute paper provides the background colour.
   * Its ink colour provides the runner's colour. It has to be
   * this way because the runner sprite sets the colours of the
   * two cells it gets placed in, and since collision detection
   * is based on paper colour the sprite will "collide with
   * itself" if the runner were to have a different background
   * colour.
   */
  uint8_t   background_att;
  uint8_t   solid_att;
  uint8_t   jumper_att;

  TILE_DEFINITION*       level_tiles;
  TELEPORTER_DEFINITION* teleporters;
  SLOWDOWN*              slowdowns;
  DOOR*       doors;

  uint16_t               max_score;
  uint16_t               max_bonus;
  SCORE_SCREEN_DATA      score_screen_data;
} LEVEL_DATA;

/*
 * Attribute of the finish block is the same on all levels so this is
 * now hardcoded.
 */
#define FINISH_ATT (INK_YELLOW|PAPER_BLUE)

#define NUM_LEVELS 5

#define START_POINT(x,y) x,y
#define LEVEL_BORDER(b) b
#define START_FACING(f) f
#define MAX_POINTS(m) m
#define MAX_BONUS(b) b
#define SLOWDOWN_SECS(s) s

void print_level_from_sp1_string(LEVEL_DATA* level_data);
void teardown_level(LEVEL_DATA* level_data);

#endif
