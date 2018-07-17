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
#include <arch/zx.h>

#include "utils.h"
#include "local_assert.h"
#include "collision.h"

#define SPRITE_WIDTH  6
#define SPRITE_HEIGHT 8

REACTION test_direction_blocked( uint8_t x, uint8_t y, DIRECTION facing, JUMP_STATUS jump_status )
{
  uint8_t  check_x;
  uint8_t  check_y;
  uint8_t* attr_address;

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
      return BOUNCE;
    }
    else {
      return NO_REACTION;
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
        return BOUNCE;
      }
      else {
        /* OK, check if he's about to bang his foot */
        check_y = y+SPRITE_HEIGHT-1;

        attr_address = zx_pxy2aaddr( check_x, check_y  );
        if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
          return BOUNCE;
        }
        else {
          return NO_REACTION;
        }
      }
      break;

    case LEFT_FLAT:
      /* Check if he's about to bang his face */
      check_x = x+1;
      check_y = y;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        return BOUNCE;
      }
      else {
        /* OK, check if he's about to bang his foot */
        check_y = y+SPRITE_HEIGHT-1;

        attr_address = zx_pxy2aaddr( check_x, check_y  );
        if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
          return BOUNCE;
        }
        else {
          return NO_REACTION;
        }
      }
      break;

    case RIGHT_RISING:
      /* Check if he's about to bang his face */
      check_x = x+SPRITE_WIDTH;
      check_y = y;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        return BOUNCE;
      }

      /* Check if he's about to bang his foot */
      check_x = x+SPRITE_WIDTH;
      check_y = y+SPRITE_HEIGHT-1;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        return BOUNCE;
      }

      /* Check if he's about to bang his head */
      check_x = x+SPRITE_WIDTH;
      check_y = y-1;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        return DROP_VERTICALLY;
      }
      else {
        return NO_REACTION;
      }
      break;

    case LEFT_RISING:
      /* Check if he's about to bang his face */
      check_x = x-1;
      check_y = y;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        return BOUNCE;
      }

      /* Check if he's about to bang his foot */
      check_x = x-1;
      check_y = y+SPRITE_HEIGHT-1;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        return BOUNCE;
      }

      /* Check if he's about to bang his head */
      check_x = x-1;
      check_y = y-1;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        return DROP_VERTICALLY;
      }
      else {
        return NO_REACTION;
      }
      break;

    case RIGHT_FALLING:
      /* Check if he's about to bang his face */
      check_x = x+SPRITE_WIDTH;
      check_y = y;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        return BOUNCE;
      }

      /* Check if he's about to bang his foot */
      check_x = x+SPRITE_WIDTH;
      check_y = y+SPRITE_HEIGHT-1;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        return BOUNCE;
      }

      return NO_REACTION;
      break;

    case LEFT_FALLING:
      /* Check if he's about to bang his face */
      check_x = x-1;
      check_y = y;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        return BOUNCE;
      }

      /* Check if he's about to bang his foot */
      check_x = x-1;
      check_y = y+SPRITE_HEIGHT-1;

      attr_address = zx_pxy2aaddr( check_x, check_y  );
      if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE ) {
        return BOUNCE;
      }

      return NO_REACTION;
      break;

    default:
      /* Unreachable code local_assert(1); */
      break;
    }
  }

  return NO_REACTION;
}
