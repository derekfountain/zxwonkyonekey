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

#ifndef __COLLECTABLE_H
#define __COLLECTABLE_H

#include <stdint.h>

/*
 * A collectable is a thing which appears on the screen and is collectable
 * by the player by walking over it. So pills, powerups, whatever.
 *
 * The interface is currently implemented using macros which is fast.
 */

typedef struct _collectable
{
  /* x,y pixel position to place the sprite */
  uint8_t            x;
  uint8_t            y;

  /*
   * The x,y position is the top left corner of the sprite. For collision purposes
   * I want to check on the centre of sprite. This location is x,y plus 3 pixels
   * (for the runner sprite) in both directions, but since it's used every frame I
   * store the values rather than do those additions every time. If different
   * sprites ever need to collide with collectable this will need a rethink.
   */
  uint8_t            centre_x;
  uint8_t            centre_y;

} COLLECTABLE;

/* Named args macro to make initialisation code easier to read */
#define INITIALISE_COLLECTABLE( xtag, x, ytag, y, xctag, xc, xytag, yc ) x,y,xc,yc

/* Macro to fetch the x,y location for a collectable's screen location */
#define COLLECTABLE_SCREEN_LOCATION(c) c.x,c.y


/***
 *      _____       _             __               
 *     |_   _|     | |           / _|              
 *       | |  _ __ | |_ ___ _ __| |_ __ _  ___ ___ 
 *       | | | '_ \| __/ _ | '__|  _/ _` |/ __/ _ \
 *      _| |_| | | | ||  __| |  | || (_| | (_|  __/
 *     |_____|_| |_|\__\___|_|  |_| \__,_|\___\___|
 *                                                 
 *                                                 
 */

/*
 * Macro answers true if the collectable is valid. That's defined as its
 * screen location not being 0,0. If I need to put a collectable at 0,0
 * this will need changing.
 */
#define IS_VALID_COLLECTABLE(collectable) (collectable.x || collectable.y)

#define IS_COLLECTION_POINT(x,y,collectable) ( (x == collectable.centre_x) && \
                                               (y == collectable.centre_y) )

#endif