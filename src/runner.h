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
#include "slowdown_pill.h"

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

  SLOWDOWN_STATUS  slowdown;
} RUNNER;


/*
 * Create the runner, initially facing in the direction given.
 */
RUNNER* create_runner( void );
void    reset_runner( DIRECTION initial_direction );

/*
 * Expose runner structure to enable getter and setter macros.
 */
extern RUNNER runner;

/*
 * These macros are faster and smaller than getter/setter funcs, but require
 * exposing the runner structure. It's worth it, the CALLs are expensive given
 * how often these are needed in the codebase. These save about 250 bytes over
 * using getter and setter functions.
 */
#define         GET_RUNNER_XPOS                ((uint8_t)(runner.xpos))
#define         GET_RUNNER_YPOS                ((uint8_t)(runner.ypos))
#define         GET_RUNNER_FACING              ((DIRECTION)(runner.facing))
#define         GET_RUNNER_JUMP_OFFSET         ((uint8_t)(runner.jump_offset))
#define         GET_RUNNER_SLOWDOWN            ((SLOWDOWN_STATUS)(runner.slowdown))

#define         SET_RUNNER_XPOS(new_pos)       (runner.xpos = (uint8_t)new_pos)
#define         SET_RUNNER_YPOS(new_pos)       (runner.ypos = (uint8_t)new_pos)
#define         SET_RUNNER_FACING(new_facing)  (runner.facing = (DIRECTION)new_facing)
#define         SET_RUNNER_SLOWDOWN(new_sd)    (runner.slowdown = (SLOWDOWN_STATUS)new_sd)

#define         MOVE_RUNNER_XPOS(delta)        (runner.xpos += (int8_t)delta)
#define         MOVE_RUNNER_YPOS(delta)        (runner.ypos += (int8_t)delta)

/* These need to stay as functions */
void            set_runner_colour( uint8_t );
JUMP_STATUS     get_runner_jump_status(void);

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
 * An action function to move the runner sideways.
 * i.e. left or right, depending on game state.
 */
PROCESSING_FLAG move_sideways( void* data, GAME_ACTION* output_action );

#endif
