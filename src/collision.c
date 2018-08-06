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

#include "collision.h"
#include "utils.h"
#include "local_assert.h"
#include "int.h"
#include "collision.h"
#include "tracetable.h"
#include "runner.h"
#include "action.h"
#include "game_state.h"

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

REACTION test_direction_blocked( uint8_t x, uint8_t y,
                                 DIRECTION facing, JUMP_STATUS jump_status, uint8_t background_att )
{
  uint8_t  check_x;
  uint8_t  check_y;
  uint8_t* attr_address;
  REACTION result;

  if( jump_status == NOT_JUMPING ) {

    /* He's not in a jump so bouncing off walls is the only thing to worry about */

    if( facing == RIGHT ) {
      check_x = x+SPRITE_WIDTH;
      check_y = y;
    }
    else { /* Facing left */
      check_x = x-1;
      check_y = y;
    }
    attr_address = zx_pxy2aaddr( check_x, check_y  );
    if( *attr_address != background_att ) {
      result = BOUNCE;
    }
    else {
      result = NO_REACTION;
    }

  }
  else {  /* In the middle of a jump */

    /*
     * TODO So much duplication here... Obvious contender for optimisation
     * when I need some cycles or space.
     */

    switch( jump_status )
    {
    case RIGHT_FLAT:
      /* Check if he's about to bang his face */
      check_x = x+SPRITE_WIDTH;
      check_y = y;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( *attr_address != background_att ) {
        result = BOUNCE;
      }
      else {
        /* OK, check if he's about to bang his foot */
        check_y = y+SPRITE_HEIGHT-1;

        attr_address = zx_pxy2aaddr( check_x, check_y  );
        if( *attr_address != background_att ) {
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
      if( *attr_address != background_att ) {
        result = BOUNCE;
      }
      else {
        /* OK, check if he's about to bang his foot */
        check_y = y+SPRITE_HEIGHT-1;

        attr_address = zx_pxy2aaddr( check_x, check_y  );
        if( *attr_address != background_att ) {
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
      if( *attr_address != background_att ) {
        result = BOUNCE;
      }
      else {

        /* Check if he's about to bang his foot */
        check_x = x+SPRITE_WIDTH;
        check_y = y+SPRITE_HEIGHT-1;

        attr_address = zx_pxy2aaddr( check_x, check_y  );
        if( *attr_address != background_att ) {
          result = BOUNCE;
        }
        else {

          /* Check if he's about to bang his head */
          check_x = x+SPRITE_WIDTH;
          check_y = y-1;

          attr_address = zx_pxy2aaddr( check_x, check_y  );
          if( *attr_address != background_att ) {
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
      if( *attr_address != background_att ) {
        result = BOUNCE;
      }
      else {

        /* Check if he's about to bang his foot */
        check_x = x-1;
        check_y = y+SPRITE_HEIGHT-1;

        attr_address = zx_pxy2aaddr( check_x, check_y  );
        if( *attr_address != background_att ) {
          result = BOUNCE;
        }
        else {

          /* Check if he's about to bang his head */
          check_x = x-1;
          check_y = y-1;

          attr_address = zx_pxy2aaddr( check_x, check_y  );
          if( *attr_address != background_att ) {
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
      if( *attr_address != background_att ) {
        result = BOUNCE;
      }
      else {

        /* Check if he's about to bang his foot */
        check_x = x+SPRITE_WIDTH;
        check_y = y+SPRITE_HEIGHT-1;

        attr_address = zx_pxy2aaddr( check_x, check_y  );
        if( *attr_address != background_att ) {
          result = BOUNCE;
        }
        else {

          /* Check if he's landed on something */
          check_x = x+SPRITE_WIDTH-1;
          check_y = y+SPRITE_HEIGHT;

          attr_address = zx_pxy2aaddr( check_x, check_y  );
          if( *attr_address != background_att ) {
            result = LANDED;
          }
        }
      }
      break;

    case LEFT_FALLING:
      /* Check if he's about to bang his face */
      check_x = x-1;
      check_y = y;

      result = NO_REACTION;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( *attr_address != background_att ) {
        result = BOUNCE;
      }
      else {

        /* Check if he's about to bang his foot */
        check_x = x-1;
        check_y = y+SPRITE_HEIGHT-1;

        attr_address = zx_pxy2aaddr( check_x, check_y  );
        if( *attr_address != background_att ) {
          result = BOUNCE;
        }
        else {

          /* Check if he's landed on something */
          check_x = x;
          check_y = y+SPRITE_HEIGHT;

          attr_address = zx_pxy2aaddr( check_x, check_y  );
          if( *attr_address != background_att ) {
            result = LANDED;
          }
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

  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t     background = game_state->current_level->background_att;

  reaction = test_direction_blocked( xpos, ypos, facing, jump_status, background );
  switch( reaction )
  {
  case BOUNCE:
    *output_action = TOGGLE_DIRECTION;
    return STOP_PROCESSING;

  case DROP_VERTICALLY:
    *output_action = STOP_JUMP;
    return STOP_PROCESSING;    

  case LANDED:
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

