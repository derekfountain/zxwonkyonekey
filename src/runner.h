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
#define NO_JUMP (uint8_t)(0xFF)

/*
 * Macro for readability. :)
 */
#define RUNNER_JUMPING(offset) (((uint8_t)(offset)) != NO_JUMP)

/*
 * The x,y position of the runner sprite is the top left corner.
 * For some types of collision detection it makes more sense to
 * test the centre point of the sprite (or as close to the 6x8
 * pixel sprite as is possible).
 */
#define RUNNER_CENTRE_X(x) (x+3)
#define RUNNER_CENTRE_Y(y) (y+4)

/*
 * Directions. Up and down may be added at some point.
 * This is currently only used for the runner, hence it's
 * in this file. If the general concept of direction is
 * needed elsewhere this will have to move.
 */
typedef enum _direction
{
  RIGHT,
  LEFT,
} DIRECTION;


/*
 * Indicator of where the player is in the animated jump sequence.
 * These are used to decide what sort of bounce or direction
 * change is appropriate if he's interrupted mid-jump.
 */
typedef enum _jump_status
{
  NOT_JUMPING,
  RIGHT_RISING,
  RIGHT_FLAT,
  RIGHT_FALLING,
  LEFT_RISING,
  LEFT_FLAT,
  LEFT_FALLING,
} JUMP_STATUS;

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

  uint8_t          slowdown;
} RUNNER;


/*
 * Create the runner, initially facing in the direction given.
 */
RUNNER* create_runner( DIRECTION initial_direction );

/*
 * Runner screen movement getters and setters.
 */
uint8_t     get_runner_xpos( void );
uint8_t     get_runner_ypos( void );
void        set_runner_xpos( uint8_t pos );
void        set_runner_ypos( uint8_t pos );
void        move_runner_xpos( int8_t delta );
void        move_runner_ypos( int8_t delta );
void        set_runner_facing( DIRECTION d );
DIRECTION   get_runner_facing( void );
void        set_runner_colour( uint8_t );
uint8_t     get_runner_jump_offset( void );
JUMP_STATUS get_runner_jump_status(void);
uint8_t     get_runner_slowdown( void );
void        set_runner_slowdown( uint8_t );


/*
 * Initialise trace table for runner
 */
void init_runner_trace(void);

/*
 * Adjust the runner's screen position depending on where he
 * is in the cycle of the jump animation, if at all.
 */
PROCESSING_FLAG adjust_for_jump(void* data, GAME_ACTION* output_action);

/*
 * Draw the runner at the screen coordinates in the controlling
 * structure
 */
void draw_runner( void );

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
 * Stop the jumping sequence.
 */
void stop_runner_jumping( void );

/*
 * Start the runner dead sequence.
 */
void runner_dead(void);

/*
 * An action function to move the runner sideways.
 * i.e. left or right, depending on game state.
 */
PROCESSING_FLAG move_sideways( void* data, GAME_ACTION* output_action );

#endif
