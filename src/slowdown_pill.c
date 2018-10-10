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
#include <arch/zx/sp1.h>

#include "slowdown_pill.h"

/* These are in the assembly language file */
extern uint8_t slowdown_pill_f1[];
extern uint8_t slowdown_pill_f2[];
extern uint8_t slowdown_pill_f3[];

/*
 * This is defined in main.c. Just share it for now.
 */
extern struct sp1_Rect full_screen;

#define SLOWDOWN_PILL_PLANE    (uint8_t)(1)

void create_slowdown_pill( SLOWDOWN_DEFINITION* slowdown )
{
  slowdown->frame     = 0;
  slowdown->expanding = 1;
  slowdown->sprite    = sp1_CreateSpr(SP1_DRAW_LOAD1LB, SP1_TYPE_1BYTE, 2, 0, SLOWDOWN_PILL_PLANE);
  sp1_AddColSpr(slowdown->sprite, SP1_DRAW_LOAD1RB, SP1_TYPE_1BYTE, 0, SLOWDOWN_PILL_PLANE);

  /* Pill sprite is created using absolute graphic data address */
  sp1_MoveSprPix(slowdown->sprite, &full_screen,
                 (int)slowdown_pill_f1,
                 slowdown->x, slowdown->y);
}

/*
 * Invalidator, called at screen update time to ensure the pill
 * sprite tiles are invalidated and hence redrawn
 */
static void invalidatePill(unsigned int count, struct sp1_update *u)
{
  (void)count;

  sp1_InvUpdateStruct(u);
}

void animate_slowdown_pill( SLOWDOWN_DEFINITION* slowdown )
{
  /*
   * Crude little state machine to pulse the slowdown pills.
   * I tried to do this with a table/data approach but it always
   * came out bigger and more complicated. :)
   */
  uint8_t* next_frame;

  if( slowdown->expanding )
  {
    if( slowdown->frame == 0 )
    {
      slowdown->frame = 1;
      next_frame = (uint8_t*)slowdown_pill_f2;
    }
    else if( slowdown->frame == 1 )
    {
      slowdown->frame = 2;
      next_frame = (uint8_t*)slowdown_pill_f3;
    }
    else /* slowdown->frame == 2 */
    {
      slowdown->expanding = 0;
      next_frame = (uint8_t*)slowdown_pill_f3;
    }
  }
  else
  {
    if( slowdown->frame == 0 )
    {
      slowdown->expanding = 1;
      next_frame = (uint8_t*)slowdown_pill_f1;
    }
    else if( slowdown->frame == 1 )
    {
      slowdown->frame = 0;
      next_frame = (uint8_t*)slowdown_pill_f1;
    }
    else /* slowdown->frame == 2 */
    {
      slowdown->frame = 1;
      next_frame = (uint8_t*)slowdown_pill_f2;
    }
  }

  /* Set the correct graphic on the pill sprite and invalidate to it redraws */
  sp1_MoveSprPix(slowdown->sprite, &full_screen, next_frame, slowdown->x, slowdown->y);
  sp1_IterateUpdateSpr(slowdown->sprite, invalidatePill);
}
