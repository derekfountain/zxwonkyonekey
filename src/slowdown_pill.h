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

#ifndef __SLOWDOWN_PILL_H
#define __SLOWDOWN_PILL_H

#include "collectable.h"

typedef enum _slowdown_status
{
  SLOWDOWN_INACTIVE,
  SLOWDOWN_ACTIVE,
} SLOWDOWN_STATUS;

/*
 * Slowdown point, stored as an x,y pixel location. This structure also
 * contains the sprite and animation bits.
 */
typedef struct _slowdown_definition
{
  COLLECTABLE        collectable;

  uint8_t            duration_secs;

  struct sp1_ss*     sprite;
  uint8_t            frame;
  uint8_t            expanding;

  uint16_t           complete_timer; 

} SLOWDOWN;

#define SLOWDOWN_SCREEN_LOCATION(slowdown) COLLECTABLE_SCREEN_LOCATION(slowdown->collectable)

/*
 * Macro answers true if the slowdown pointed to is valid. At the moment
 * that's defined as the collectable being valid.
 */
#define IS_VALID_SLOWDOWN(slowdown) (IS_VALID_COLLECTABLE(slowdown->collectable))

void create_slowdown_pill( SLOWDOWN* slowdown );
void destroy_slowdown_pill( SLOWDOWN* slowdown );
void animate_slowdown_pill( SLOWDOWN* slowdown );

void slowdown_collected(COLLECTABLE* collectable, void* data);

#endif
