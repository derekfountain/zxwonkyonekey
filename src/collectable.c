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

#include "int.h"
#include "tracetable.h"
#include "collectable.h"

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
 * This defines the collectable's trace table.
 */

/* BE:PICKUPDEF */
#define COLLECTABLE_TRACE_ENTRIES 120
#define COLLECTABLE_TRACETABLE_SIZE ((size_t)sizeof(COLLECTABLE_TRACE)*COLLECTABLE_TRACE_ENTRIES)

TRACE_FN( collectable, COLLECTABLE_TRACE, COLLECTABLE_TRACETABLE_SIZE )

void COLLECTABLE_TRACE_CREATE(COLLECTABLE_TRACETYPE ttype, COLLECTABLE* cptr, uint8_t x, uint8_t y)
{
  if( collectable_tracetable != TRACING_INACTIVE )
  {
    COLLECTABLE_TRACE     ct;           
    ct.ticker           = GET_TICKER;   
    ct.tracetype        = ttype;        
    ct.collectable      = cptr;         
    ct.xpos             = cptr->timer_countdown/256;            
    ct.ypos             = cptr->timer_countdown%256;            
    collectable_add_trace(&ct);         
  }
}

void init_collectable_trace(void)
{
  if( collectable_tracetable == TRACING_UNINITIALISED )
    collectable_tracetable = collectable_next_trace = allocate_tracememory(COLLECTABLE_TRACETABLE_SIZE);
}
