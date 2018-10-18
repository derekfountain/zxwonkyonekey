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
 *
 */
typedef struct _door_definition
{
  COLLECTABLE        collectable;

  /*
Collectable works. It's not quite right, the interface needs to be generic
enough to work on a door or a slowdown pill, but it's getting there. Keys
are going to be collectable too, so I think I need to build a linked list
of collectables in the collectable code which I can walk each game loop
checking if the collectable is at the runner location. If it is run the
collectable's collection function, which has to differ - keys will do
something different to slowdown pills when collected.

I used a non animated sprite for the key, which I need to make flicker using
an attribute loop. The door needs to be a sprite (it's currently a UGD) so
it can be pixel positioned. It doesn't need animation, but Y coord is
necessary. X coord too if I make them able to be trapdoors. So maybe a
Flickerable class? Probably not, at the moment it's hard to see the reuse case.

OTOH there's common ground here. The pills pulse. The keys will flicker. Loop
over collectables, call their animation function if there is one?
   */

  /*
   * TODO
   * Active (or permanently open)
   * Cell x,y
   * "Under" range x-x,y
   * Door colour
   * Timer
   * Animation timer
   * Animation door y position
   * 
   */
} DOOR_DEFINITION;


#endif
