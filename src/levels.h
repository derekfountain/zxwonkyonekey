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

/*
 * Teleporter moves the runner from one screen location to another.
 * The x,y coords are pixels, required for testing against where
 * the sprite is. The x_cell,y_cell coords are cells, required for
 * tile invalidation. Both coord types are required each game loop
 * so it makes sense to store both types rather than calculate them
 * each time round.
 * Some teleporters change the runner's direction, so he might go in
 * facing left and come out facing right. That's the flag at the end.
 */
typedef struct _teleporter_defintion
{
  uint8_t end_1_y;
  uint8_t end_1_x;

  uint8_t end_1_y_cell;
  uint8_t end_1_x_cell;

  uint8_t end_2_y;
  uint8_t end_2_x;

  uint8_t end_2_y_cell;
  uint8_t end_2_x_cell;

  uint8_t change_direction;
} TELEPORTER_DEFINITION;


/*
 * Tile defintion is essentially a user defined graphic for the sp1 library.
 * Tile numbers for UDGs start at 128.
 */
typedef struct _tile_definition
{
  uint8_t  tile_num;
  uint8_t* udg_data;
} TILE_DEFINITION;


typedef struct _level_data
{
  void      (*draw_func)(struct _level_data*);
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
  uint8_t   teleporter_att;
  uint8_t   finish_att;

  TILE_DEFINITION*       level_tiles;
  TELEPORTER_DEFINITION* teleporters;

  SCORE_SCREEN_DATA      score_screen_data;
} LEVEL_DATA;

#define NUM_LEVELS 3

#define START_POINT(x,y) x,y
#define LEVEL_BORDER(b) b
#define START_FACING(f) f

LEVEL_DATA* get_level_data( uint8_t level );

#endif
