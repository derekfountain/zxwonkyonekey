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

/*
 * The pill sits closer to the viewer and is OR'ed into the display.
 * The runner is behind, OR'ed in, the pill is OR'ed over the top.
 * The runner used to be LOAD'ed, these separate planes are probably
 * now redundant.
 */
#define SLOWDOWN_PILL_PLANE    (uint8_t)(1)

void create_slowdown_pill( SLOWDOWN* slowdown )
{
  slowdown->frame     = 0;
  slowdown->expanding = 1;
  slowdown->sprite    = sp1_CreateSpr(SP1_DRAW_OR1LB, SP1_TYPE_1BYTE, 2, 0, SLOWDOWN_PILL_PLANE);
  sp1_AddColSpr(slowdown->sprite, SP1_DRAW_OR1RB, SP1_TYPE_1BYTE, 0, SLOWDOWN_PILL_PLANE);

  /*
   * Pill sprite is created using absolute graphic data address.
   * I use the _callee version specifically here because sp1_MoveSprPix()
   * is itself a macro and putting the SLOWDOWN_SCREEN_LOCATION macro
   * in the arguments breaks the preprocessor.
   */
  sp1_MoveSprPix_callee(slowdown->sprite, &full_screen,
                        (void*)slowdown_pill_f1,
                        SLOWDOWN_SCREEN_LOCATION(slowdown));
}

void destroy_slowdown_pill( SLOWDOWN* slowdown )
{
  /* Move sprite offscreen before calling delete function */
  sp1_MoveSprPix(slowdown->sprite, &full_screen, (void*)0, 255, 255);
  sp1_DeleteSpr(slowdown->sprite);
}

/*
 * Invalidator, called at screen update time to ensure the pill
 * sprite tiles are invalidated and hence redrawn
 */
static void invalidatePillSprite(unsigned int count, struct sp1_update *u)
{
  (void)count;

  sp1_InvUpdateStruct(u);
}

void animate_slowdown_pill( SLOWDOWN* slowdown )
{
  /*
   * Crude little state machine to pulse the slowdown pills.
   * I tried to do this with a table/data approach but it always
   * came out bigger and more complicated. :)
   */
  uint8_t* next_frame;

  if( !IS_COLLECTABLE_AVAILABLE(slowdown->collectable) )
  {
    /* Move it off screen so it disappears */
    next_frame = (uint8_t*)slowdown_pill_f1;
    sp1_MoveSprPix(slowdown->sprite, &full_screen, next_frame, 255, 255);
  }
  else
  {
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

    /* Set the correct graphic on the pill sprite */
    sp1_MoveSprPix_callee(slowdown->sprite, &full_screen,
                          next_frame,
                          SLOWDOWN_SCREEN_LOCATION(slowdown));
  }

  /* Finally, invalidate the pill sprite so it redraws */
  sp1_IterateUpdateSpr(slowdown->sprite, invalidatePillSprite);
}


/*
 * Collectable collection handler, called when the the runner
 * collects the slowdown pill. Set the pill unavailable and
 * update the screen accordingly, then start the timer to count
 * down the time the runner is moving slowly.
 */
void slowdown_collected(COLLECTABLE* collectable, void* data)
{
  SLOWDOWN* slowdown = (SLOWDOWN*)data;
  (void)collectable;

  SET_COLLECTABLE_AVAILABLE(slowdown->collectable,COLLECTABLE_NOT_AVAILABLE);

  /*
   * Bit of a design breakage here. Updating screen data in a collectable
   * handler. But it's simple and readable, and the option is to create
   * a new gameloop action function to do it, which would be wasteful.
   * Do it this way for now.
   */
  animate_slowdown_pill( slowdown );

  START_COLLECTABLE_TIMER(slowdown->collectable,slowdown->duration_secs);

  return;
}

