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

#ifndef __TELEPORTER_H
#define __TELEPORTER_H

#include <stdint.h>

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

#endif
