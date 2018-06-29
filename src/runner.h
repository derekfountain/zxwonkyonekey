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

#ifndef __RUNNER_H
#define __RUNNER_H

#include <stdint.h>

#include "action.h"

/*
 * Jump offset value is an index into an array. This value
 * means not jumping.
 */
#define NOT_JUMPING (uint8_t)(0xFF)

/*
 * Macro for readability. :)
 */
#define RUNNER_JUMPING(offset) (((uint8_t)(offset)) != NOT_JUMPING)

/*
 * Directions. Up and down may be added at some point.
 * This is currently only used for the runner, hence it's
 * in this file. If the general concept of direction is
 * needed elsewhere this will have to move.
 */
typedef enum _direction
{
  RIGHT=0,
  LEFT =1,
}
DIRECTION;

/*
 * This structure defines the runner - i.e. the player's
 * character. It should be a singleton but that's not enforced
 * in order to save a bit of space.
 */
typedef struct _runner
{
  struct sp1_ss*   sprite;

  uint8_t          xpos;
  uint8_t          ypos;

  DIRECTION        facing;
  uint8_t          jump_offset;
}
RUNNER;


/*
 * Create the runner, initially facing in the direction given.
 */
RUNNER* create_runner( DIRECTION initial_direction );

/*
 * Position the runner at the given screen coordinates.
 *
 * The runner code holds a structure with the character's
 * nominal screen position, but it's this code which places
 * him on screen, taking into account jumping and whatever
 * else might modify his actual x,y screen position.
 *
 * y-pos is a pointer because the positioning code takes
 * jumping into account and may have to move the sprite
 * above the requested position. In this case the sprite's
 * actual y-coordinate is returned.
 */
/* TODO I need to fix this offset thing. Currently the y position
 of the runner, which is traced, isn't the y position at all if
 he's jumping!
*/
void position_runner( uint8_t x, uint8_t* y );

/*
 * Switch the runner's direction, left or right. If up and
 * down become possible this will need rethinking.
 */
void toggle_runner_direction( void );

/*
 * Start the jumping sequence.
 */
void start_runner_jumping( void );

/*
 * An action function to move the runner sideways.
 * i.e. left or right, depending on game state.
 */
GAME_ACTION move_sideways( void* data );

#endif
