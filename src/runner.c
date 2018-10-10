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

#include <stdint.h>
#include <string.h>
#include <arch/zx.h>
#include <arch/zx/sp1.h>

#include "runner.h"
#include "game_state.h"
#include "tracetable.h"
#include "int.h"
#include "utils.h"
#include "collision.h"

extern uint8_t runner_right_f1[];
extern uint8_t runner_right_f2[];
extern uint8_t runner_right_f3[];
extern uint8_t runner_right_f4[];
extern uint8_t runner_right_f5[];
extern uint8_t runner_right_f6[];
extern uint8_t runner_right_f7[];
extern uint8_t runner_right_f8[];

extern uint8_t runner_left_f1[];
extern uint8_t runner_left_f2[];
extern uint8_t runner_left_f3[];
extern uint8_t runner_left_f4[];
extern uint8_t runner_left_f5[];
extern uint8_t runner_left_f6[];
extern uint8_t runner_left_f7[];
extern uint8_t runner_left_f8[];

/***
 *      _______             _             
 *     |__   __|           (_)            
 *        | |_ __ __ _  ___ _ _ __   __ _ 
 *        | | '__/ _` |/ __| | '_ \ / _` |
 *        | | | | (_| | (__| | | | | (_| |
 *        |_|_|  \__,_|\___|_|_| |_|\__, |
 *                                   __/ |
 *                                  |___/ 
 *
 * This defines the runner's trace table.
 */

typedef enum _runner_tracetype
{
  JUMP_START,
  JUMP_STOP,
  JUMPING_UPWARDS,
  JUMPING_DOWNWARDS,
  JUMP_LAST,
} RUNNER_TRACETYPE;

typedef struct _runner_trace
{
  uint16_t           ticker;
  RUNNER_TRACETYPE   tracetype;
  uint8_t            xpos;
  uint8_t            ypos;
  int8_t            ydelta;
} RUNNER_TRACE;

/* BE:PICKUPDEF */
#define RUNNER_TRACE_ENTRIES 50
#define RUNNER_TRACETABLE_SIZE ((size_t)sizeof(RUNNER_TRACE)*RUNNER_TRACE_ENTRIES)

TRACE_FN( runner, RUNNER_TRACE, RUNNER_TRACETABLE_SIZE )

void init_runner_trace(void)
{
  if( runner_tracetable == TRACING_UNINITIALISED )
    runner_tracetable = runner_next_trace = allocate_tracememory(RUNNER_TRACETABLE_SIZE);
}

/*
 * Filling in a blank trace entry is normally done with a macro,
 * but since this one is called several times a function is more
 * economical.
 */
void RUNNER_TRACE_CREATE( RUNNER_TRACETYPE ttype, uint8_t x, uint8_t y, int8_t yd )
{
  if( runner_tracetable != TRACING_INACTIVE ) {
    RUNNER_TRACE  rt;		  
    rt.ticker     = GET_TICKER;			
    rt.tracetype  = ttype; 
    rt.xpos       = x;	 
    rt.ypos       = y;	  
    rt.ydelta     = yd;	     
    runner_add_trace(&rt);			
  }						
}

/*
 * Runner has access to full screen for now. It'll be faster
 * when I can reduce this window size.
 */
const struct sp1_Rect runner_screen = {0, 0, 32, 24};


/***
 *           _                        __     __   ____   __  __          _       
 *          | |                       \ \   / /  / __ \ / _|/ _|        | |      
 *          | |_   _ _ __ ___  _ __    \ \_/ /  | |  | | |_| |_ ___  ___| |_ ___ 
 *      _   | | | | | '_ ` _ \| '_ \    \   /   | |  | |  _|  _/ __|/ _ | __/ __|
 *     | |__| | |_| | | | | | | |_) |    | |    | |__| | | | | \__ |  __| |_\__ \
 *      \____/ \__,_|_| |_| |_| .__/     |_|     \____/|_| |_| |___/\___|\__|___/
 *                            | |                                                
 *                            |_|                                                
 * Table of offsets in the y-position the runner takes as he
 * goes through the jump animation.
 * This data set makes the jump 5 chars wide, with a sharp
 * incline at the start and decline at the end, so like an
 * arc. Arrived at emprically.
 */
const int8_t jump_y_offsets[40] =  { 2,  2,  2,  2,    2,  1,  1,  1,
			             1,  1,  1,  1,    1,  1,  1,  0,
                                     0,  0,  0,  0,    0,  0,  0,  0,
		                     0, -1, -1, -1,   -1, -1, -1, -1,
			            -1, -1, -1, -2,   -2, -2, -2, -2 };

/*
 * Size in bytes of one frame of the runner sprite graphic. It's currently
 * bytes of data plus 8 bytes of padding.
 *
 * For flexibility I want to do something like:
 *
 *  static const uint16_t runner_frame_size = runner_right_f2-runner_right_f1;
 *
 * but the compiler complains that those values aren't constants. So maybe
 * there's a way to export a DEFC'ed value from the assembler to the C?
 * I can't find a way to do that either. So for now use a hard coded constant.
 *
 * Futhermore, this:
 *
 *   static const uint16_t runner_frame_size = 16;
 * 
 * actually compiles to a value held in memory, which is a bit crap.
 * Using a constant of 16 compiles to 4 'add hl,hl' instructions.
 */
#define RUNNER_FRAME_SIZE (uint16_t)16


/*
 * Structure to look after the runner character.
 */
RUNNER runner;

#define RUNNER_PLANE    (uint8_t)(2)

RUNNER* create_runner( DIRECTION initial_direction )
{
  runner.sprite = sp1_CreateSpr(SP1_DRAW_LOAD1LB, SP1_TYPE_1BYTE, 2, 0, RUNNER_PLANE);
  sp1_AddColSpr(runner.sprite, SP1_DRAW_LOAD1RB, SP1_TYPE_1BYTE, 0, RUNNER_PLANE);

  /*
   * The sprite is actually 6 pixels wide, not 8, so it can be rotated
   * 2 pixels horizontally without crossing into the next cell and requiring
   * the rightmost column to be drawn. The x-threshold is therefore 3:
   * the rightmost column needs drawing if the sprite is rotated 3 more
   * more pixels.
   */
  runner.sprite->xthresh = 3;

  /* These will be initialised from level data. Just make them sane here. */
  runner.xpos        = 0;
  runner.ypos        = 0;
  runner.facing      = initial_direction;

  runner.jump_offset = NO_JUMP;

  return &runner;
}

uint8_t get_runner_xpos( void )
{
  return runner.xpos;
}

uint8_t get_runner_ypos( void )
{
  return runner.ypos;
}

void set_runner_xpos( uint8_t pos )
{
  runner.xpos = pos;
}

void set_runner_ypos( uint8_t pos )
{
  runner.ypos = pos;
}

void move_runner_xpos( int8_t delta )
{
  runner.xpos += delta;
}

void move_runner_ypos( int8_t delta )
{
  runner.ypos += delta;
}

void set_runner_facing( DIRECTION dir )
{
  runner.facing = dir;
}

DIRECTION get_runner_facing( void )
{
  return runner.facing;
}

static uint8_t required_colour;
static void initialiseColour(unsigned int count, struct sp1_cs *c)
{
  (void)count;    /* Suppress compiler warning about unused parameter */

  c->attr_mask = SP1_AMASK_INK|SP1_AMASK_PAPER;
  c->attr      = required_colour;
}

void set_runner_colour( uint8_t new_att )
{
  required_colour = new_att;
  sp1_IterateSprChar(runner.sprite, initialiseColour);
}

uint8_t get_runner_jump_offset( void )
{
  return runner.jump_offset;
}

JUMP_STATUS get_runner_jump_status( void )
{
  if( RUNNER_JUMPING(runner.jump_offset) ) {

    int8_t y_delta = jump_y_offsets[runner.jump_offset];
    if( y_delta < 0 ) {
      if( runner.facing == RIGHT ) {
	return RIGHT_FALLING;
      }
      else {
	return LEFT_FALLING;
      }
    }
    else if( y_delta == 0 ) {
      if( runner.facing == RIGHT ) {
	return RIGHT_FLAT;
      }
      else {
	return LEFT_FLAT;
      }
    } else {
      if( runner.facing == RIGHT ) {
	return RIGHT_RISING;
      }
      else {
	return LEFT_RISING;
      }
    }
  }
  else {
    return NOT_JUMPING;
  }
}


PROCESSING_FLAG adjust_for_jump(void* data, GAME_ACTION* output_action)
{
  (void)data;  /* Unused parameter, stop the compiler warning */

  if( RUNNER_JUMPING(runner.jump_offset) ) {
    int8_t y_delta = jump_y_offsets[runner.jump_offset];

    /*
     * If the last entry in the jump y-offsets table has been used
     * he's no longer jumping.
     */
    if( ++runner.jump_offset == sizeof(jump_y_offsets) ) {
      runner.jump_offset = NO_JUMP;

      /* Note that this trace is logged *before* the final y adjustment */
      RUNNER_TRACE_CREATE(JUMP_LAST, runner.xpos, runner.ypos, y_delta);
    }
    else {

      /*
       * If the jump y-delta is negative he's in the second half of the jump.
       * OTOH if the jump y-delta is positive he's in the first half of the
       */
      if( y_delta < 0 ) {

	RUNNER_TRACE_CREATE(JUMPING_DOWNWARDS, runner.xpos, runner.ypos, y_delta);
      }
      else {
	RUNNER_TRACE_CREATE(JUMPING_UPWARDS, runner.xpos, runner.ypos, y_delta);
      }
    }

    runner.ypos -= y_delta;
  }

  *output_action = NO_ACTION;
  return KEEP_PROCESSING;
}


void draw_runner(void)
{
  uint8_t* runner_data;
  uint16_t offset_to_frame;

  /*
   * Frame number is calculated from the lowest 8 bits of the x position.
   * This won't be flexible enough if the animation gets more complex
   * but for now it's fast and simple.
   *
   * The calculation is, for example:
   * 
   *  frame_num   = x & 0x0007;
   *  runner_data = runner_right_f1+(RUNNER_FRAME_SIZE*frame_num);
   *
   * The implementation below doesn't use the intermediate variable and
   * generates more succinct code.
   */
  offset_to_frame = RUNNER_FRAME_SIZE * (runner.xpos & 0x0007);

  if( runner.facing == RIGHT ) {
    runner_data = runner_right_f1+offset_to_frame;
  }
  else {
    runner_data = runner_left_f1+offset_to_frame;
  }

  sp1_MoveSprPix(runner.sprite, &runner_screen, runner_data, runner.xpos, runner.ypos);
}


void toggle_runner_direction(void)
{
  /*
   * As long as there are only two directions enumerated as 0 and 1,
   * this 'not' is OK. :)
   */
  runner.facing = !runner.facing;

  /*
   * If the runner is mid-jump, see if he's more than halfway
   * through. Adjust the position of the jump position
   * accordingly to allow the jump to be completed after he's
   * turned around in midair. This was determined empirically;
   * the resultant animation and positioning feel right.
   */
  if( RUNNER_JUMPING(runner.jump_offset) )
    if( runner.jump_offset && (runner.jump_offset < sizeof(jump_y_offsets)/2) )
      runner.jump_offset = sizeof(jump_y_offsets) - runner.jump_offset;
}


void start_runner_jumping(void)
{
  runner.jump_offset = 0;

  RUNNER_TRACE_CREATE(JUMP_START, 0, 0, 0);
}


void stop_runner_jumping(void)
{
  runner.jump_offset = NO_JUMP;

  RUNNER_TRACE_CREATE(JUMP_STOP, 0, 0, 0);
}


void runner_dead(void)
{
  zx_border(INK_RED);
  while(1);
}


PROCESSING_FLAG move_sideways(void* data, GAME_ACTION* output_action)
{
  (void)data;  /* Unused parameter, stop the compiler warning */

  if( runner.facing == RIGHT ) {
    *output_action = MOVE_RIGHT;
  }
  else {
    *output_action = MOVE_LEFT;
  }
  return KEEP_PROCESSING;
}
