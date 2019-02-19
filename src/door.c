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
#include <arch/zx/sp1.h>

#include "door.h"
#include "utils.h"
#include "int.h"
#include "tracetable.h"
#include "game_state.h"
#include "graphics.h"

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
 * This defines the door's trace table.
 */

typedef enum _door_tracetype
{
  DOOR_CREATED,
  DOOR_DESTROYED,
  DOOR_ANIMATED,
  DOOR_KEY_COLLECTED,
  DOOR_TIMEOUT,
  DOOR_PASSED_THROUGH,
} DOOR_TRACETYPE;

typedef struct _door_trace
{
  uint16_t           ticker;
  DOOR_TRACETYPE     tracetype;
  DOOR*              door;
} DOOR_TRACE;

/* BE:PICKUPDEF */
#define DOOR_TRACE_ENTRIES 120
#define DOOR_TRACETABLE_SIZE ((size_t)sizeof(DOOR_TRACE)*DOOR_TRACE_ENTRIES)

/* It's quicker to do this with a macro, as long as it's only used once or twice */
#define DOOR_TRACE_CREATE(ttype,dptr) {     \
    if( door_tracetable != TRACING_INACTIVE ) { \
      DOOR_TRACE          dt;   \
      dt.ticker           = GET_TICKER; \
      dt.tracetype        = ttype; \
      dt.door             = dptr; \
      door_add_trace(&dt); \
    } \
}

TRACE_FN( door, DOOR_TRACE, DOOR_TRACETABLE_SIZE )

void init_door_trace(void)
{
  door_tracetable = door_next_trace = allocate_tracememory(DOOR_TRACETABLE_SIZE);
}


/* This is in the assembly language file */
extern uint8_t door_f1[];

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

/*
 * Key graphic is a static tile (a UDG). It's displayed as a tile via
 * the SP1 print routine. It must be the same ink/paper colour as the
 * runner otherwise he'll bounce off it.
 */
static uint8_t key_sp1_string[9] = "\x16" "yx" "\x10" "i" "\x11" "p" "t";
void display_key( DOOR* door, uint8_t visible )
{
  /* Build and print the string to display the key */
  key_sp1_string[1] = door->collectable.y;
  key_sp1_string[2] = door->collectable.x;
  key_sp1_string[4] = door->key_ink;
  key_sp1_string[6] = door->key_paper;

  if( visible )
    key_sp1_string[7] = KEY_TILE_NUM;
  else
    key_sp1_string[7] = KEY_BLANK_TILE_NUM;

  sp1_PrintString(&level_print_control, key_sp1_string);
}


static uint8_t ink_param;
static void initialise_colour(unsigned int count, struct sp1_cs *c)
{
  (void)count;    /* Suppress compiler warning about unused parameter */

  c->attr_mask = SP1_AMASK_INK;
  c->attr      = ink_param;
}

void create_door( DOOR* door )
{
  display_key( door, TRUE );

  door->moving   = DOOR_STATIONARY;
  door->y_offset = 0;

  door->sprite = sp1_CreateSpr(SP1_DRAW_LOAD1LB, SP1_TYPE_1BYTE, 2, 0, DOOR_PLANE);
  sp1_AddColSpr(door->sprite, SP1_DRAW_LOAD1RB, SP1_TYPE_1BYTE, 0, DOOR_PLANE);

  /*
   * I use the _callee version specifically here because sp1_MoveSprPix()
   * is itself a macro and putting the DOOR_SCREEN_LOCATION macro
   * in the arguments breaks the preprocessor.
   */
  sp1_MoveSprPix_callee(door->sprite, &full_screen,
                        (void*)door_f1,
                        DOOR_SCREEN_LOCATION(door));

  /* Colour the cells the sprite occupies */
  ink_param = door->door_ink_colour;
  sp1_IterateSprChar(door->sprite, initialise_colour);

  DOOR_TRACE_CREATE(DOOR_CREATED,door);

  if( door->start_open_secs )
  {
    door_key_collected(&door->collectable, door);
    START_COLLECTABLE_TIMER(door->collectable, door->start_open_secs);
  }

  COLLECTABLE_TRACE_CREATE( COLLECTABLE_CREATED, &(door->collectable), GET_RUNNER_XPOS, GET_RUNNER_YPOS );
}

void destroy_door( DOOR* door )
{
  COLLECTABLE_TRACE_CREATE(COLLECTABLE_TO_BE_DESTROYED, &(door->collectable), GET_RUNNER_XPOS, GET_RUNNER_YPOS );

  /* Move sprite offscreen before calling delete function */
  sp1_MoveSprPix(door->sprite, &full_screen, (void*)0, 255, 255);
  sp1_DeleteSpr(door->sprite);

  DOOR_TRACE_CREATE(DOOR_DESTROYED,door);
}

void animate_door( DOOR* door )
{
  if( door->moving == DOOR_OPENING )
  {
    if( ++(door->y_offset) == 8 )
      door->moving = DOOR_STATIONARY;

  }
  else
  {
    if( --(door->y_offset) == 0 )
      door->moving = DOOR_STATIONARY;

  }

  sp1_MoveSprPix_callee(door->sprite, &full_screen,
                        (void*)door_f1,
                        DOOR_SCREEN_LOCATION_WITH_OFFSET(door));

  DOOR_TRACE_CREATE(DOOR_ANIMATED,door);
}

void animate_door_key( DOOR* door )
{
  /* Call key display key using availability as visibility */
  display_key( door, IS_COLLECTABLE_AVAILABLE( door->collectable ) );
}

/*
 * Collectable collection handler, called when the the runner
 * collects the door key. Set the key unavailable and
 * update the screen accordingly, then start the timer to count
 * down the time the door is open.
 */
void door_key_collected(COLLECTABLE* collectable, void* data)
{
  DOOR* door = (DOOR*)data;
  (void)collectable;

  SET_COLLECTABLE_AVAILABLE(door->collectable,COLLECTABLE_NOT_AVAILABLE);

  /* Remove key from screen */
  animate_door_key( door );

  /* Open the door */
  door->moving = DOOR_OPENING;
  door->animation_step = 0;

  START_COLLECTABLE_TIMER(door->collectable, door->open_secs);

  COLLECTABLE_TRACE_CREATE( COLLECTABLE_COLLECTED, &(door->collectable), GET_RUNNER_XPOS, GET_RUNNER_YPOS );
  DOOR_TRACE_CREATE(DOOR_KEY_COLLECTED,door);

  return;
}

/*
 * Door open time up handler, called by the collectable timer timeout
 * code when the user has collected the key and the door has been open
 * for the allotted period. Close the door and reinstate the key.
 */
uint8_t door_open_timeup(COLLECTABLE* collectable, void* data)
{
  DOOR* door = (DOOR*)data;
  (void)collectable;

  SET_COLLECTABLE_AVAILABLE(door->collectable,COLLECTABLE_AVAILABLE);

  /* Redraw the key */
  animate_door_key( door );

  /* Close the door */
  door->moving = DOOR_CLOSING;
  door->animation_step = 0;

  COLLECTABLE_TRACE_CREATE( COLLECTABLE_TIMEOUT, &(door->collectable), GET_RUNNER_XPOS, GET_RUNNER_YPOS );
  DOOR_TRACE_CREATE(DOOR_TIMEOUT,door);

  return 0;
}

void check_door_passed_through( DOOR* door )
{
  uint8_t xpos = GET_RUNNER_XPOS;
  uint8_t ypos = GET_RUNNER_YPOS;

  if( IS_DOOR_PASSTHROUGH_POINT( door, xpos, ypos ) )
  {
    /*
     * Cancel the timer with the door open and the key off screen.
     * The door stays open.
     */
    CANCEL_COLLECTABLE_TIMER( door->collectable );
    COLLECTABLE_TRACE_CREATE( COLLECTABLE_TIMEOUT, &(door->collectable), xpos, ypos );

    DOOR_TRACE_CREATE(DOOR_PASSED_THROUGH,door);
  }
  
}


void validate_door_cells( DOOR* first_door )
{
  DOOR* door = first_door;

  while( IS_VALID_COLLECTABLE(door->collectable) )
  {
    /*
     * There doesn't appear to be an SP1 interface to validate a
     * single tile. Use a 1x1 rectangle instead.
     */
    struct sp1_Rect validate_cell = {door->door_protected_cell_y,
                                     door->door_protected_cell_x, 1, 1};
    sp1_Validate(&validate_cell);
    
    door++;
  }
}
