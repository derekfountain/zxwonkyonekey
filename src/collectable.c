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
#include "collectable.h"

#if 0
void handle_timed_collectable( COLLECTABLE* collectable )
{
  /*
   * If the pill has been consumed and is active, reduce the
   * timer. If that's got to zero, reinstate the pill.
   */
  if( !IS_COLLECTABLE_AVAILABLE(collectable) )
  {
    DECREMENT_COLLECTABLE_TIMER( collectable );

    if( COLLECTABLE_TIMER_EXPIRED( collectable ) )
    {
      // KEY_ACTION_TRACE_CREATE( SLOWDOWN_EXPIRED, 0 );

      /*
       * The return value of the timeout function is taken to indicate
       * whether the slowdown mode should be deactivated.
       */
      if( (*(collectable->timer_fn))( collectable, (void*)slowdown ) )
        *output_action = DEACTIVATE_SLOWDOWN;
      else
        *output_action = NO_ACTION;
    }

  }
  else
  {
    /*
     * Pill is available. If he's walked onto it call the hander.
     */
    if( IS_COLLECTION_POINT( RUNNER_CENTRE_X(xpos),
                             RUNNER_CENTRE_Y(ypos), collectable) )
    {
      // KEY_ACTION_TRACE_CREATE( CONSUMED_SLOWDOWN, 0 );

      /*
       * The handler currently returns void. This could be changed to
       * return a flag to activate (or not) the slowdown. This is not
       * currently required.
       */
      (*(collectable->collection_fn))( collectable, (void*)slowdown );

      *output_action = ACTIVATE_SLOWDOWN;
      break;
    }
  }
  
}
#endif
