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

#include "collectable.h"

/*
 * A door is made up of a key, which is a tile, and a door which moves
 * when the key is collected. The door is a sprite.
 */
typedef struct _door
{
  COLLECTABLE        collectable;

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

  /*
I used a non animated sprite for the key, which I need to make flicker using
an attribute loop. The door needs to be a sprite (it's currently a UGD) so
it can be pixel positioned. It doesn't need animation, but Y coord is
necessary. X coord too if I make them able to be trapdoors.

OTOH there's common ground here. The pills pulse. The keys will flicker. Loop
over collectables, call their animation function if there is one?
   */

  /*
   * TODO
   * Cell x,y
   * "Under" range x-x,y
   * Door colour
   * Animation timer
   * Animation door y position
   * 
   */
} DOOR;


/*
 * Macro answers true if the door pointed to is valid.
 * That's defined as the collectable being valid.
 */
#define IS_VALID_DOOR(door) (IS_VALID_COLLECTABLE(door->collectable))

void create_door( DOOR* door );
void destroy_door( DOOR* door );

void door_key_collected(COLLECTABLE* collectable, void* data);
uint8_t door_open_timeup(COLLECTABLE* collectable, void* data);


#endif
