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

/*
 * Tracing. I wrote an article here: http://www.derekfountain.org/spectrum_ffdc.php
 * which describes the idea behind this. This implementation is standardised
 * using macros so the tools all Just Work(TM).
 *
 * Examine the tracing in a Spectrum memory dump with:
 *
 * >be -i wonkyonekey.berc -y wonky.sym dump.ss@0
 *
 */

#ifndef __TRACETABLE_H
#define __TRACETABLE_H

#include <unistd.h>

#define TRACING_UNINITIALISED ((void*)0xFFFF)
#define TRACING_INACTIVE      ((void*)0xFFFE)

/*
 * Start of memory area used for trace table. Spectrum's ROM starts
 * at zero but I can't use byte zero because BE thinks that's the
 * result of a null dereference. So use 1.
 */
#define TRACE_MEMORY_START ((uint16_t)1)

/*
 * Maximum amount of memory to allocate to tracetables.
 * We use the ROM area, so 16K minus the 1 discussed above.
 */
#define MAX_TRACE_MEMORY ((uint16_t)16383)

/*
 * Macro to generate a function to add an entry to a trace table.
 * The process is to take a pointer to a structure which defines
 * the trace entry, memcpy() it into the next slot of the trace
 * table, advance the next slot point, and if it wraps put the
 * next slot pointer back to the start of the table. Since all
 * tracing needs to do exactly this, this function is generated
 * with a macro to enforce conformity.
 *
 * The macro takes the name of the thing to be traced, the type
 * which defines the trace entry structure, and the number of
 * those entries in the table. It defines and initialises the
 * table pointer and the next entry in the table pointer.
 */
#define TRACE_FN( NAME, TYPE, NUM_ENTRIES )	\
\
TYPE * NAME ## _tracetable = TRACING_UNINITIALISED; \
TYPE * NAME ## _next_trace = 0xFFFF; \
\
void NAME ## _add_trace( TYPE * ptr ) \
{\
  memcpy( NAME ## _next_trace, ptr, sizeof(TYPE));\
\
  NAME ## _next_trace = (void*)((uint8_t*)NAME ## _next_trace + sizeof(TYPE));\
\
  if( NAME ## _next_trace == (void*)((uint8_t*)NAME ## _tracetable + NUM_ENTRIES) )\
      NAME ## _next_trace = NAME ## _tracetable;\
}


/*
 * Find out if the ROM is writable. In some emulators it can be.
 * Answers 1 if it is, or 0 if not.
 */
uint8_t is_rom_writable(void);

/*
 * Clear or otherwise initialise the area of memory all the
 * tracing will go into.
 */
void* clear_trace_area(void);

/*
 * Allocate memory to hold a tracetable of 'size' bytes.
 * Returns a pointer to the zeroth byte (i.e. the first
 * trace entry).
 */
void* allocate_tracememory( size_t size );

#endif
