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

#ifndef __DOOR_H
#define __DOOR_H

#include <stdint.h>

#include "action.h"
#include "collectable.h"

/*
 * Enum indicates which way a door is moving - opening or closing
 */
typedef enum _door_moving
{
  DOOR_STATIONARY,
  DOOR_OPENING,
  DOOR_CLOSING,
} DOOR_MOVING;

/*
 * Enum indicates the direction a door opens. Most are like a
 * portcullis, opening upwards. Some are trapdoors, moving
 * sideways.
 */
typedef enum _door_opening_direction
{
  OPENS_BOTTOM_TO_TOP,
  OPENS_LEFT_TO_RIGHT,
  OPENS_RIGHT_TO_LEFT,
} DOOR_OPENING_DIRECTION;

/*
 * A door is made up of a key, which is a tile, and a door which moves
 * when the key is collected. The door is a sprite.
 */
typedef struct _door
{
  COLLECTABLE        collectable;

  uint8_t            door_cell_x;
  uint8_t            door_cell_y;

  /*
   * The door sprite animates the opening/closing by moving the sprite
   * to a new pixel location. In the typical case the door moves into the
   * cell above it; that cell is given here. This cell is validated each
   * loop so the SP1 engine doesn't update it. This makes the door sprite
   * "disappear" into the cell.
   */
  uint8_t            door_protected_cell_x;
  uint8_t            door_protected_cell_y;

  /*
   * If the runner hits this point on screen he's deemed to have got to
   * the door while it's open, and hence the door stays open
   */
  uint8_t            door_stays_open_x;
  uint8_t            door_stays_open_y;

  uint8_t            door_ink_colour;

  /*
   * Key is a tile, so this is SP1 cell information
   */
  uint8_t            key_ink;
  uint8_t            key_paper;

  /*
   * Keys are displayed using a single tile - a UDG in effect. Which UDG depends on
   * how the UDGs are set up for the level, so the actual key UDG is in the door
   * definition.
   */
  uint8_t            key_tile_num;

  /*
   * Number of seconds the door stays open. The actual timer is in the collectable.
   * It's initialised to this value
   */
  uint8_t            open_secs;
  uint8_t            start_open_secs;

  /* Stuff below here isn't initialised at compile time */

  /*
   * Door is a sprite because it needs to be pixel-positioned as part of the
   * opening/closing animation
   */
  struct sp1_ss*     sprite;

  /* Which way, if any, the door is currently moving (opening or closing) */
  DOOR_MOVING        moving;
  uint8_t            animation_step;

  /* Sprite is 8 pixels high/wide, so this goes 0-7 and the door moves */
  uint8_t            y_offset;

} DOOR;


/*
 * Macro answers true if the door pointed to is valid.
 * That's defined as the collectable being valid.
 */
#define IS_VALID_DOOR(door) (IS_VALID_COLLECTABLE(door->collectable))

#define DOOR_IS_OPEN(door) (door->moving==DOOR_STATIONARY && door->y_offset==8)
#define DOOR_IS_MOVING(door) (door->moving!=DOOR_STATIONARY)

#define IS_DOOR_PASSTHROUGH_POINT(door,x,y) ((door->door_stays_open_x == x) && \
                                             (door->door_stays_open_y == y))
/*
 * These multiplications might be a bit expensive? I can restructure the data
 * to get rid of them if I need to.
 */
#define DOOR_SCREEN_LOCATION(door)              door->door_cell_x*8,door->door_cell_y*8
#define DOOR_SCREEN_LOCATION_WITH_OFFSET(door)  door->door_cell_x*8,(door->door_cell_y*8)-door->y_offset

void create_door( DOOR* door );
void destroy_door( DOOR* door );
void animate_door( DOOR* door );

void door_key_collected(COLLECTABLE* collectable, void* data);
uint8_t door_open_timeup(COLLECTABLE* collectable, void* data);

void validate_door_cells( DOOR* first_door );
void check_door_passed_through( DOOR* door );

#endif
