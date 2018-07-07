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
