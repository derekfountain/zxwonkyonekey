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

#ifndef __COLLISION_H
#define __COLLISION_H

#include "runner.h"
#include "action.h"

typedef enum _reaction
{
  NO_REACTION,
  BOUNCE,
  DROP_VERTICALLY,
  LANDED,
} REACTION;


typedef enum _corner
{
  TOP_RIGHT,
  BOTTOM_RIGHT,
  BOTTOM_LEFT,
  TOP_LEFT,
} CORNER;


/*
 * Initialise trace table for collision detection
 */
void init_collision_trace(void);

/*
 * Action function to decide whether the player has collided with something
 * and what to do about it.
 */
PROCESSING_FLAG act_on_collision( void* data, GAME_ACTION* output_action );

#endif
