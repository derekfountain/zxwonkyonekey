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

#include "utils.h"
#include "local_assert.h"
#include "int.h"
#include "collision.h"
#include "tracetable.h"
#include "runner.h"

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
 * This defines the collision checker trace table.
 */

typedef enum _collision_tracetype
{
  CHECK_BLOCKED,
} COLLISION_TRACETYPE;

typedef struct _collision_trace
{
  uint16_t            ticker;
  COLLISION_TRACETYPE tracetype;
  uint8_t             xpos;
  uint8_t             ypos;
  DIRECTION           direction;
  JUMP_STATUS         jump_status;
  REACTION            reaction;
} COLLISION_TRACE;

#define COLLISION_TRACE_ENTRIES 250
#define COLLISION_TRACETABLE_SIZE ((size_t)sizeof(COLLISION_TRACE)*COLLISION_TRACE_ENTRIES)

COLLISION_TRACE* collision_tracetable = TRACING_UNINITIALISED;
COLLISION_TRACE* collision_next_trace = 0xFFFF;

#define COLLISION_TRACE_CREATE(ttype,x,y,d,js,r) {    \
    if( collision_tracetable != TRACING_INACTIVE ) { \
      COLLISION_TRACE      ct;   \
      ct.ticker          = GET_TICKER; \
      ct.tracetype       = ttype; \
      ct.xpos            = x; \
      ct.ypos            = y; \
      ct.direction       = d; \
      ct.jump_status     = js; \
      ct.reaction        = r; \
      collision_add_trace(&ct); \
    } \
}

void collision_add_trace( COLLISION_TRACE* ct_ptr )
{
  memcpy(collision_next_trace, ct_ptr, sizeof(COLLISION_TRACE));

  collision_next_trace = (void*)((uint8_t*)collision_next_trace + sizeof(COLLISION_TRACE));

  if( collision_next_trace == (void*)((uint8_t*)collision_tracetable+COLLISION_TRACETABLE_SIZE) )
      collision_next_trace = collision_tracetable;
}

void init_collision_trace(void)
{
  if( collision_tracetable == TRACING_UNINITIALISED )
    collision_tracetable = collision_next_trace = allocate_tracememory(COLLISION_TRACETABLE_SIZE);
}


#define SPRITE_WIDTH  6
#define SPRITE_HEIGHT 8

REACTION test_direction_blocked( uint8_t x, uint8_t y, DIRECTION facing, JUMP_STATUS jump_status )
{
  uint8_t  check_x;
  uint8_t  check_y;
  uint8_t* attr_address;
  REACTION result;

  if( jump_status == NOT_JUMPING ) {

    if( facing == RIGHT ) {
      check_x = x+SPRITE_WIDTH;
      check_y = y;
    }
    else { /* Facing left */
      check_x = x-1;
      check_y = y;
    }
    attr_address = zx_pxy2aaddr( check_x, check_y  );
    if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
      result = BOUNCE;
    }
    else {
      result = NO_REACTION;
    }

  }
  else {  /* In the middle of a jump */

    switch( jump_status )
    {
    case RIGHT_FLAT:
      /* Check if he's about to bang his face */
      check_x = x+SPRITE_WIDTH;
      check_y = y;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        result = BOUNCE;
      }
      else {
        /* OK, check if he's about to bang his foot */
        check_y = y+SPRITE_HEIGHT-1;

        attr_address = zx_pxy2aaddr( check_x, check_y  );
        if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
          result = BOUNCE;
        }
        else {
          result = NO_REACTION;
        }
      }
      break;

    case LEFT_FLAT:
      /* Check if he's about to bang his face */
      check_x = x+1;
      check_y = y;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        result = BOUNCE;
      }
      else {
        /* OK, check if he's about to bang his foot */
        check_y = y+SPRITE_HEIGHT-1;

        attr_address = zx_pxy2aaddr( check_x, check_y  );
        if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
          result = BOUNCE;
        }
        else {
          result = NO_REACTION;
        }
      }
      break;

    case RIGHT_RISING:
      /* Check if he's about to bang his face */
      check_x = x+SPRITE_WIDTH;
      check_y = y;

      result = NO_REACTION;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        result = BOUNCE;
      }
      else {

        /* Check if he's about to bang his foot */
        check_x = x+SPRITE_WIDTH;
        check_y = y+SPRITE_HEIGHT-1;

        attr_address = zx_pxy2aaddr( check_x, check_y  );
        if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
          result = BOUNCE;
        }
        else {

          /* Check if he's about to bang his head */
          check_x = x+SPRITE_WIDTH;
          check_y = y-1;

          attr_address = zx_pxy2aaddr( check_x, check_y  );
          if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
            result = DROP_VERTICALLY;
          }
        }
      }
      break;

    case LEFT_RISING:
      /* Check if he's about to bang his face */
      check_x = x-1;
      check_y = y;

      result = NO_REACTION;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        result = BOUNCE;
      }
      else {

        /* Check if he's about to bang his foot */
        check_x = x-1;
        check_y = y+SPRITE_HEIGHT-1;

        attr_address = zx_pxy2aaddr( check_x, check_y  );
        if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
          result = BOUNCE;
        }
        else {

          /* Check if he's about to bang his head */
          check_x = x-1;
          check_y = y-1;

          attr_address = zx_pxy2aaddr( check_x, check_y  );
          if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
            result = DROP_VERTICALLY;
          }
        }
      }
      break;

    case RIGHT_FALLING:
      /* Check if he's about to bang his face */
      check_x = x+SPRITE_WIDTH;
      check_y = y;

      result = NO_REACTION;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        result = BOUNCE;
      }
      else {

        /* Check if he's about to bang his foot */
        check_x = x+SPRITE_WIDTH;
        check_y = y+SPRITE_HEIGHT-1;

        attr_address = zx_pxy2aaddr( check_x, check_y  );
        if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
          result = BOUNCE;
        }
      }
      break;

    case LEFT_FALLING:
      /* Check if he's about to bang his face */
      check_x = x-1;
      check_y = y;

      result = NO_REACTION;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        result = BOUNCE;
      }
      else {

      /* Check if he's about to bang his foot */
        check_x = x-1;
        check_y = y+SPRITE_HEIGHT-1;

        attr_address = zx_pxy2aaddr( check_x, check_y  );
        if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
          result = BOUNCE;
        }
      }
      break;

    default:
      /* Unreachable code local_assert(1); */
      break;
    }
  }

  COLLISION_TRACE_CREATE( CHECK_BLOCKED, x, y, facing, jump_status, result);

  return result;
}

PROCESSING_FLAG act_on_collision( void* data, GAME_ACTION* output_action )
{
  REACTION    reaction;

  uint8_t     xpos = get_runner_xpos();
  uint8_t     ypos = get_runner_ypos();
  DIRECTION   facing = get_runner_facing();
  DIRECTION   jump_status = get_runner_jump_status();

  (void)data;

  reaction = test_direction_blocked( xpos, ypos, facing, jump_status );
  switch( reaction )
  {
  case BOUNCE:
    *output_action = TOGGLE_DIRECTION;
    return STOP_PROCESSING;

  case DROP_VERTICALLY:
    *output_action = STOP_JUMP;
    return STOP_PROCESSING;    
  }

  *output_action = NO_ACTION;
  return KEEP_PROCESSING;
}

#if 0
This can be useful when trying to see what is where

void plot(unsigned char x, unsigned char y)
{
  *zx_pxy2saddr(x,y) |= zx_px2bitmask(x);
}
#endif


#if 0
Not sure there is any use for this...

void find_corner_pixel( uint8_t* x, uint8_t* y, CORNER corner );
void find_corner_pixel( uint8_t* x, uint8_t* y, CORNER corner )
{
  *x = runner.xpos;
  *y = runner.ypos;

  switch( corner )
  {
  case TOP_RIGHT:
    *x += 6;
    *y -= 1;
    return;
    
  case BOTTOM_RIGHT:
    *x += 6;
    *y += 8;
    return;

  case TOP_LEFT:
    *x -= 1;
    *y -= 1;
    return;

  case BOTTOM_LEFT:
    *x -= 1;
    *y += 8;
    return;
  }
}
#endif

#if 0
Broken idea. Keep for now, some of the code might be reusable.

#include <stdio.h>
PROCESSING_FLAG update_xy_delta( void* data, GAME_ACTION* output_action )
{
  uint8_t        xpos;
  uint8_t        ypos;
  DIRECTION      facing      = get_runner_facing();
  JUMP_STATUS    jump_status = get_runner_jump_status();

  uint8_t*       attr_address;

  (void)data;

  /*
   * Check if he's about to bang his head on a block. This applies if he's
   * on the flat, or in any part of his jump.
   */
  find_corner_pixel( &xpos, &ypos, (facing==RIGHT) ? TOP_RIGHT : TOP_LEFT );

//  printf("js=%u, x=%u y=%u\n",jump_status,xpos, ypos);
  plot(xpos, ypos);
  zx_border(INK_YELLOW);

  attr_address = zx_pxy2aaddr( xpos, ypos );
  if( (*attr_address & ATTR_MASK_PAPER) == PAPER_WHITE ) {

    /* Empty cell in front - he can move into it */

//  printf("js=%u, x=%u y=%u\n",jump_status,xpos, ypos);
    switch( jump_status )
    {
    case NOT_JUMPING:
      *output_action = (facing==RIGHT) ? MOVE_RIGHT : MOVE_LEFT;
      return STOP_PROCESSING;      
    }
  }
  else {
    *output_action = TOGGLE_DIRECTION;
    return STOP_PROCESSING;      
  }
//  printf("here\n");

  /*
   * If he's in the falling part of his jump also check if he's about to bang
   * his foot into a block
   */
  if( jump_status == RIGHT_FALLING || jump_status ==  LEFT_FALLING ) {
    find_corner_pixel( &xpos, &ypos, (facing==RIGHT) ? BOTTOM_RIGHT : BOTTOM_LEFT );
    attr_address = zx_pxy2aaddr( xpos, ypos );
    if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
    }
  }

//  sprite_corner = find_corner_pixel( &xpos, &ypos,  );

  

  /*
i think this has to be an update_xy_deltas(). I can't do one then the other.


i need to work out the leading pixel based on his movement and then work
out if that pixel is in a free block.

this is called before the sprite is moved.

so:
  moving right, leading edge is top right most pixel of sprite 
    moving into next right pixel
    toggle direction if pixel is block
      stop processing
  moving left, leading edge is top left most pixel of sprite 
    moving into next left pixel
    toggle direction if pixel is block
      stop processing
  falling vertically, leading edge is bottom left pixel of sprite
    moving into next lower pixel
    no action if pixel is block so he stops falling
      stop processing
  moving right, rising (i.e. jumping up right), leading edge is top right most pixel of sprite 
    moving into next right, up one or two pixels
    invert the jump - toggle direction and switch to falling
    stop processing
  moving left, rising (i.e. jumping up left), leading edge is top left most pixel of sprite 
    moving into next left, up one or two pixels
    invert the jump - toggle direction and switch to falling
    stop processing
  moving right, falling (i.e. jumping down to right), leading edge is bottom right most pixel of sprite 
    moving into next right, down one pixel
    invert the jump - toggle direction
    stop processing
  moving left, falling (i.e. jumping down to left), leading edge is bottom left most pixel of sprite 
    moving into next left, down one pixel
    invert the jump - toggle direction
    stop processing
  rising vertically we don't do

so, calculate first pixel to move into new place
find attribute of that cell
decide whether it can happen

DOESN'T WORK
imagine he's falling from his jump. he needs to go down one and along one.
if the block beneath his is empty he can go down one. if i then find the
block in front of him is not empty i can't move him along one.
if jumping
  if rising side of the jump
    if facing left
      if att directly above is empty
        move_up
      else
        stop jump, he should now fall vertically
     else facing right
       if att above and right is empty
        move_up
      else
        stop jump, he should now fall vertically

  else if flat part of jump
    return no_action, keep processing

  else if falling side of the jump
    if facing left
      
    else facing right

else if no ground below feet
  This is the falling code i already have
  Might return move_down, if it does stop processing so he doesn't move sideways


  */
  return KEEP_PROCESSING;
}
*/
#endif