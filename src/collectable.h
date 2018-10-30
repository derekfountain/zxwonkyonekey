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

typedef enum _collectable_availability
{
  COLLECTABLE_NOT_AVAILABLE,
  COLLECTABLE_DISARMED,
  COLLECTABLE_AVAILABLE,
} COLLECTABLE_AVAILABILITY;

typedef struct _collectable
{
  /*
   * x,y position to place the sprite. Some collectables use this as a
   * pixel, others use it as cell coordinates
   */
  uint8_t                   x;
  uint8_t                   y;

  /*
   * The x,y position is the top left corner of the sprite. For collision purposes
   * I want to check on the centre of sprite. This location is x,y in pixels, plus
   * 3 pixels (for the runner sprite) in both directions, but since it's used every
   * frame I store the values rather than do those additions every time. If different
   * sprites ever need to collide with collectables this will need a rethink.
   */
  uint8_t                   centre_x;
  uint8_t                   centre_y;

  /*
   * Is available? This typically starts as yes, and is set to no when it gets
   * collected.
   */
  COLLECTABLE_AVAILABILITY  available;

  /* Function to call when collected */
  void                      (*collection_fn)(struct _collectable*, void*);

  /* Countdown to timer expiring in 1/50ths sec */
  uint16_t                  timer_countdown;

  /* Function to call when countdown timer expires */
  uint8_t                   (*timer_fn)(struct _collectable*, void*);

} COLLECTABLE;

/* Named args macro to make initialisation code easier to read */
#define INITIALISE_COLLECTABLE( xtag, x, ytag, y, xctag, xc, xytag, yc, cfn, tfn) \
  x,y,xc,yc,COLLECTABLE_AVAILABLE,cfn,0,tfn

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

/*
 * TODO These take a pointer to the collectable, a change I made in an attempt at genericisation.
 * I think this is unnecessary, so they can go back to structures, removing the need for a
 * bunch of dereferencing. Check when the doors are done.
 */

/*
 * Collection point is the x,y pixel location of the collectable.
 */
#define IS_COLLECTION_POINT(x,y,collectable) ( (x == ((COLLECTABLE*)collectable)->centre_x) && \
                                               (y == ((COLLECTABLE*)collectable)->centre_y) )

/*
 * "Available" is a simple flag in the structure.
 */
#define IS_COLLECTABLE_AVAILABLE(collectable) (((COLLECTABLE*)collectable)->available == COLLECTABLE_AVAILABLE)
#define SET_COLLECTABLE_AVAILABLE(collectable,a) ((COLLECTABLE*)collectable)->available=a

/*
 * Collectables typically start a countdown timer.
 */
#define START_COLLECTABLE_TIMER(collectable,secs) ((COLLECTABLE*)collectable)->timer_countdown=(secs*50)
#define DECREMENT_COLLECTABLE_TIMER(collectable) (--(((COLLECTABLE*)collectable)->timer_countdown))
#define CANCEL_COLLECTABLE_TIMER(collectable) (((COLLECTABLE*)collectable)->timer_countdown=0)
#define COLLECTABLE_TIMER_EXPIRED(collectable) (((COLLECTABLE*)collectable)->timer_countdown==0)

uint8_t handle_timed_collectable( COLLECTABLE* collectable, void* data );

#endif
