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
#include <sound.h>

#include "utils.h"
#include "int.h"
#include "tracetable.h"
#include "runner.h"
#include "sound.h"
#include "bonus.h"
#include "slowdown_pill.h"
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
 * This defines the slowdown pill's trace table.
 */

typedef enum _slowdown_tracetype
{
  SLOWDOWN_CREATED,
  SLOWDOWN_DESTROYED,
  SLOWDOWN_ANIMATED,
  SLOWDOWN_COLLECTED,
  SLOWDOWN_TIMEOUT,
} SLOWDOWN_TRACETYPE;

typedef struct _slowdown_trace
{
  uint16_t           ticker;
  SLOWDOWN_TRACETYPE tracetype;
  SLOWDOWN*          slowdown;
  uint8_t            num_active_slowdowns;
  uint8_t            slowdowns_disabled;
} SLOWDOWN_TRACE;

/* BE:PICKUPDEF */
#define SLOWDOWN_TRACE_ENTRIES 120
#define SLOWDOWN_TRACETABLE_SIZE ((size_t)sizeof(SLOWDOWN_TRACE)*SLOWDOWN_TRACE_ENTRIES)

/* It's quicker to do this with a macro, as long as it's only used once or twice */
#define SLOWDOWN_TRACE_CREATE(ttype,sptr,n,d) {     \
    if( slowdown_tracetable != TRACING_INACTIVE ) { \
      SLOWDOWN_TRACE            st;   \
      st.ticker               = GET_TICKER; \
      st.tracetype            = ttype; \
      st.slowdown             = sptr; \
      st.num_active_slowdowns = n; \
      st.slowdowns_disabled   = d; \
      slowdown_add_trace(&st); \
    } \
}

TRACE_FN( slowdown, SLOWDOWN_TRACE, SLOWDOWN_TRACETABLE_SIZE )

void init_slowdown_trace(void)
{
  slowdown_tracetable = slowdown_next_trace = allocate_tracememory(SLOWDOWN_TRACETABLE_SIZE);
}



/* These are in the assembly language file */
extern uint8_t slowdown_pill_f1[];
extern uint8_t slowdown_pill_f2[];
extern uint8_t slowdown_pill_f3[];

/*
 * Game-wide slowdown disabling. This is set when too many slowdowns have
 * been consumed. All slowdowns disappear and become unusable.
 */
uint8_t slowdowns_disabled = 0;

/*
 * There can be more than one slowdown pill active. i.e. he consumes
 * one pill and its timer starts, then he consumes another pill before
 * the first one has expired. Don't deactivate the slowdown until both
 * timers have completed. I just keep an active timer counter. When this
 * goes back to zero there are no active timers and slowdown mode should
 * be cancelled.
 */
uint8_t num_active_slowdowns = 0;

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
  SET_COLLECTABLE_AVAILABLE(slowdown->collectable,COLLECTABLE_AVAILABLE);
  slowdown->sprite    = sp1_CreateSpr(SP1_DRAW_OR1LB, SP1_TYPE_1BYTE, 2, 0, SLOWDOWN_PILL_PLANE);
  sp1_AddColSpr(slowdown->sprite, SP1_DRAW_OR1RB, SP1_TYPE_1BYTE, 0, SLOWDOWN_PILL_PLANE);

  SLOWDOWN_TRACE_CREATE(SLOWDOWN_CREATED,slowdown,num_active_slowdowns,slowdowns_disabled);

  /*
   * Pill sprite is created using absolute graphic data address.
   * I use the _callee version specifically here because sp1_MoveSprPix()
   * is itself a macro and putting the SLOWDOWN_SCREEN_LOCATION macro
   * in the arguments breaks the preprocessor.
   */
  if( slowdowns_disabled )
    sp1_MoveSprPix_callee(slowdown->sprite, &full_screen,
                          (void*)slowdown_pill_f1,
                          255,255);
  else
    sp1_MoveSprPix_callee(slowdown->sprite, &full_screen,
                          (void*)slowdown_pill_f1,
                          SLOWDOWN_SCREEN_LOCATION(slowdown));

  COLLECTABLE_TRACE_CREATE( COLLECTABLE_CREATED, &(slowdown->collectable), 0, 0 );
}

/*
 * Slowdown pill destructor. This reclaims the graphics structures.
 * The levels are designed so slowdown mode is not possible when
 * the runner gets to the finish, but I cancel the timer anyway,
 * just in case.
 */
void destroy_slowdown_pill( SLOWDOWN* slowdown )
{
  COLLECTABLE_TRACE_CREATE(COLLECTABLE_TO_BE_DESTROYED, &(slowdown->collectable), 0, 0 );

  /* If the timer is active, cancel it */
  if( !COLLECTABLE_TIMER_EXPIRED( slowdown->collectable ) ) {
    CANCEL_COLLECTABLE_TIMER( slowdown->collectable );
    num_active_slowdowns--;
  }

  /* Move sprite offscreen before calling delete function */
  sp1_MoveSprPix(slowdown->sprite, &full_screen, (void*)0, 255, 255);
  sp1_DeleteSpr(slowdown->sprite);

  SLOWDOWN_TRACE_CREATE(SLOWDOWN_DESTROYED,slowdown,num_active_slowdowns,slowdowns_disabled);
}

/*
 * Invalidator, called at screen update time to ensure the pill
 * sprite tiles are invalidated and hence redrawn
 */
static void invalidate_pill_sprite(unsigned int count, struct sp1_update *u)
{
  (void)count;

  sp1_InvUpdateStruct(u);
}

/*
 * This is called every few hundred millisecs to "pulse" the pills
 * on screen.
 */
void animate_slowdown_pill( SLOWDOWN* slowdown )
{
  /*
   * Crude little state machine to pulse the slowdown pills.
   * I tried to do this with a table/data approach but it always
   * came out bigger and more complicated. :)
   */
  uint8_t* next_frame;

  if( (!IS_COLLECTABLE_AVAILABLE( slowdown->collectable )) || slowdowns_disabled )
  {
    /* Move it off screen so it disappears */
    next_frame = (uint8_t*)slowdown_pill_f1;
    sp1_MoveSprPix(slowdown->sprite, &full_screen, next_frame, 255, 255);

    COLLECTABLE_TRACE_CREATE( COLLECTABLE_UNANIMATE, &(slowdown->collectable), GET_RUNNER_XPOS, GET_RUNNER_YPOS );
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

    SLOWDOWN_TRACE_CREATE(SLOWDOWN_ANIMATED,slowdown,num_active_slowdowns,slowdowns_disabled);

    COLLECTABLE_TRACE_CREATE( COLLECTABLE_ANIMATE, &(slowdown->collectable), GET_RUNNER_XPOS, GET_RUNNER_YPOS );
  }

  /* Finally, invalidate the pill sprite so it redraws */
  sp1_IterateUpdateSpr(slowdown->sprite, invalidate_pill_sprite);
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

  num_active_slowdowns++;
  START_COLLECTABLE_TIMER(slowdown->collectable,slowdown->duration_secs);

  lose_bonus();
  queue_beepfx_sound(BEEPFX_JUMP_2);
  SET_RUNNER_SLOWDOWN( SLOWDOWN_ACTIVE );

  COLLECTABLE_TRACE_CREATE( COLLECTABLE_COLLECTED, &(slowdown->collectable), GET_RUNNER_XPOS, GET_RUNNER_YPOS );
  SLOWDOWN_TRACE_CREATE(SLOWDOWN_COLLECTED,slowdown,num_active_slowdowns,slowdowns_disabled);

  return;
}

uint8_t slowdown_timeup(COLLECTABLE* collectable, void* data)
{
  SLOWDOWN* slowdown = (SLOWDOWN*)data;
  (void)collectable;

  SET_COLLECTABLE_AVAILABLE(slowdown->collectable,COLLECTABLE_AVAILABLE);

  /*
   * Update screen. This should really be done in the gameloop
   * but that only updates every few hundred millisecs to animate
   * the pulsing. I could create a new gameloop action but it
   * seems harmless to do this update here, even if it does break
   * the design a bit.
   */
  animate_slowdown_pill( slowdown );

  SET_RUNNER_SLOWDOWN( SLOWDOWN_INACTIVE );

  COLLECTABLE_TRACE_CREATE( COLLECTABLE_TIMEOUT, &(slowdown->collectable), GET_RUNNER_XPOS, GET_RUNNER_YPOS );
  SLOWDOWN_TRACE_CREATE(SLOWDOWN_TIMEOUT,slowdown,num_active_slowdowns,slowdowns_disabled);

  /*
   * If this was the last active slowdown, return true in order to
   * deactivate slowdown mode. Otherwise there's still at least one
   * more active slowdown, so return false to stay in slowdown mode.
   */
  --num_active_slowdowns;

  return ( num_active_slowdowns == 0 );
}

