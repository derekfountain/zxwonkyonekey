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

#define RUNNER_TRACE_ENTRIES 50
#define RUNNER_TRACETABLE_SIZE ((size_t)sizeof(RUNNER_TRACE)*RUNNER_TRACE_ENTRIES)

RUNNER_TRACE* runner_tracetable = TRACING_UNINITIALISED;
RUNNER_TRACE* runner_next_trace = 0xFFFF;

#if 0
/*
 * This macro works best if there are only 1 or 2 tracing points, otherwise
 * it produces quite a lot of inline code. In that case a subroutine call
 * is more efficient (slower, but uses much less memory).
 */
#define RUNNER_TRACE_CREATE(ttype,x,y,yd) {	  \
    if( runner_tracetable != TRACING_INACTIVE ) { \
      RUNNER_TRACE  rt; \
      rt.ticker     = GET_TICKER; \
      rt.tracetype  = ttype; \
      rt.xpos       = x; \
      rt.ypos       = y; \
      rt.ydelta     = yd; \
      runner_add_trace(&rt); \
    } \
}
#endif

void runner_add_trace( RUNNER_TRACE* rt_ptr )
{
  memcpy(runner_next_trace, rt_ptr, sizeof(RUNNER_TRACE));

  runner_next_trace = (void*)((uint8_t*)runner_next_trace + sizeof(RUNNER_TRACE));

  if( runner_next_trace == (void*)((uint8_t*)runner_tracetable+RUNNER_TRACETABLE_SIZE) )
    runner_next_trace = runner_tracetable;
}

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
 * The negative values are probably a bit expensive on the
 * Z80 but this appears to be the easiest way to do it.
 */
const int8_t jump_y_offsets[] =  { 2,  2,  2,  2,    2,  1,  1,  1,
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
static RUNNER runner;

RUNNER* create_runner( DIRECTION initial_direction )
{
  if( runner_tracetable == TRACING_UNINITIALISED )
    runner_tracetable = runner_next_trace = allocate_tracememory(RUNNER_TRACETABLE_SIZE);

  runner.sprite = sp1_CreateSpr(SP1_DRAW_LOAD1LB, SP1_TYPE_1BYTE, 2, 0, 0);
  sp1_AddColSpr(runner.sprite, SP1_DRAW_LOAD1RB, SP1_TYPE_1BYTE, 0, 0);

  /* These will be initialised from level data. Just make them sane here. */
  /* TODO Direction needs to come from the level data too, so I need a runner init() function */
  runner.xpos        = 0;
  runner.ypos        = 0;
  runner.facing      = initial_direction;

  runner.jump_offset = NOT_JUMPING;

  return &runner;
}


void position_runner( uint8_t x, uint8_t* y )
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
  offset_to_frame = RUNNER_FRAME_SIZE * (x & 0x0007);

  if( runner.facing == RIGHT ) {
    runner_data = runner_right_f1+offset_to_frame;
  }
  else {
    runner_data = runner_left_f1+offset_to_frame;
  }

  if( RUNNER_JUMPING(runner.jump_offset) ) {
    int8_t y_delta = jump_y_offsets[runner.jump_offset];

    /*
     * If the last entry in the jump y-offsets table has been used
     * he's no longer jumping.
     */
    if( ++runner.jump_offset == sizeof(jump_y_offsets) ) {
      runner.jump_offset = NOT_JUMPING;

      /* Note that this trace is logged *before* the final y adjustment */
      RUNNER_TRACE_CREATE(JUMP_LAST, x, *y, y_delta);
    }
    else {
      uint8_t*    attr_address;

      /*
       * If the jump y-delta is negative he's in the second half of the jump.
       * i.e. falling. That means he could land on something.
       * OTOH if the jump y-delta is positive he's in the first half of the
       * jump. i.e. rising. That means he could hit his head on something.
       */
      if( y_delta < 0 ) {

	RUNNER_TRACE_CREATE(JUMPING_DOWNWARDS, x, *y, y_delta);

	/* If there's something solid under him, stop the jump */
	if( MODULO8(*y) == 0 ) {
	  attr_address = zx_pxy2aaddr( x, (*y)+8  );

	  /*
	   * TODO Something not quite right here. When he's falling he's probably moving
	   * forwards too. If he hits something in front he should bounce off it, as
	   * opposed to fall through it which is what currently happens.
	   * I think the bounce code in test_for_direction_change() needs to check whether
	   * he's not y-aligned, and if not check both cells he's facing. Needs working
	   * out though.
	   */
  
	  if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
	    y_delta = 0;
	    runner.jump_offset = NOT_JUMPING;
	  }
	}
      }
      else {
	RUNNER_TRACE_CREATE(JUMPING_UPWARDS, x, *y, y_delta);

	/* If there's something solid above him, stop the jump */
	if( MODULO8(*y) == 0 ) {
	  attr_address = zx_pxy2aaddr( x, (*y)-8  );
  
	  if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
	    y_delta = 0;
	    runner.jump_offset = NOT_JUMPING;
	  }
	}

      }
    }

    *y -= y_delta;
  }

  sp1_MoveSprPix(runner.sprite, &runner_screen, runner_data, x, *y);
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
    if( runner.jump_offset < sizeof(jump_y_offsets)/2 )
      runner.jump_offset = sizeof(jump_y_offsets) - runner.jump_offset;
}


void start_runner_jumping(void)
{
  runner.jump_offset = 0;

  RUNNER_TRACE_CREATE(JUMP_START, 0, 0, 0);
}


void runner_dead(void)
{
  zx_border(INK_RED);
  while(1);
}


GAME_ACTION move_sideways( void* data )
{
  GAME_STATE* game_state = (GAME_STATE*)data;

  if( game_state->runner->facing == RIGHT ) {
    return MOVE_RIGHT;
  }
  else {
    return MOVE_LEFT;
  }
}
