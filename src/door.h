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
enough to work on a door or a slowdown pill, but it's getting there.

I need an animated sprite for the key, plus a non animated one for the door.

Then I need to share the slowdown pill code which draws the pill graphic, I want
to reuse it for the key. Then make the collection code generic so it's reused
for the key collection.
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
