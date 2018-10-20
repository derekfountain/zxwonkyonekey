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

#include "door.h"

/*
 * This is defined in main.c. Just share it for now.
 */
extern struct sp1_Rect full_screen;

/*
 * This is in the levels code. It can only be used after the levels code
 * has initialised it.
 */
extern struct sp1_pss level_print_control;

/*
 * The door sits closer to the viewer and is OR'ed into the display.
 * The runner is behind, LOAD'ed in so that's fast, the door is OR'ed
 * over the top.
 */
#define DOOR_PLANE    (uint8_t)(1)

static uint8_t key_sp1_string[9] = "\x16" "yx" "\x10" "i" "\x11" "p" "t";
void create_door( DOOR_DEFINITION* door )
{
  /* Build and print the string to display the key */
  key_sp1_string[1] = door->key_y;
  key_sp1_string[2] = door->key_x;
  key_sp1_string[4] = door->key_ink;
  key_sp1_string[6] = door->key_paper;
  key_sp1_string[7] = door->key_tile_num;

  sp1_PrintString(&level_print_control, key_sp1_string);

  /*
   * Key is a tile, placed at its x,y cell. It's not a static thing like a jumper
   * so it's probably best handled here.
   *
   * The door is a sprite, placed at a certain location. It's not animated like
   * a pulsing pill, but it will need to be repositioned based on a timer.
   *
   * This code will need a timer, so I need to break that out and share it
   * with the teleporter code.
   *
   * The key is a collectable, so maybe I need to add that to the gameloop function
   * which detects pills. There's not going to be pills and keys in the same
   * location so I can run both loops and break as soon as one of them returns
   * a hit.
   *
   * The door needs a 60ms timer. Check github issue, I worked it out.
   *
   * Door sprite is a sprite, I need to get that in the SP1 graphics defs.
   */
#if 0
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
#endif
}

void destroy_door( DOOR_DEFINITION* door )
{
#if 0
  /* Move sprite offscreen before calling delete function */
  sp1_MoveSprPix(slowdown->sprite, &full_screen, (void*)0, 255, 255);
  sp1_DeleteSpr(slowdown->sprite);
#endif
}

/*
 * Invalidator, called at screen update time to ensure the door
 * sprite tiles are invalidated and hence redrawn
 */
static void invalidateDoorSprite(unsigned int count, struct sp1_update *u)
{
  (void)count;

  sp1_InvUpdateStruct(u);
}

void animate_door( DOOR_DEFINITION* door )
{
#if 0
  /*
   * Crude little state machine to pulse the slowdown pills.
   * I tried to do this with a table/data approach but it always
   * came out bigger and more complicated. :)
   */
  uint8_t* next_frame;

  if( slowdown->available == PILL_NOT_AVAILABLE )
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
#endif
}
