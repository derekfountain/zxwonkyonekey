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

#include <stddef.h>
#include <stdint.h>
#include <z80.h>
#include <string.h>

#include "tracetable.h"

/*
 * TODO
 * Still not quite sure how this is going to hang together.
 * I can't return "inactive" if there are several tables active
 * but no room for one more. I need an index of where they all
 * are so I can find them in the memory map. Etc.
 */

static uint8_t* tracetable_head = TRACE_MEMORY_START;

void* allocate_tracememory( size_t size )
{
  void* allocated_block;

  if( (!is_rom_writable()) || (size_t)tracetable_head + size > MAX_TRACE_MEMORY )
    return TRACING_INACTIVE;

  allocated_block = tracetable_head;
  tracetable_head += size;

  return allocated_block;
}

void* clear_trace_area(void)
{
  memset((void*)TRACE_MEMORY_START, 0xDF, MAX_TRACE_MEMORY);
}

/*
 * Utility function to see if the ROM area is writable. In this program
 * the ROM area is used for a tracetable, so this is a reasonable place
 * for this function.
 */
uint8_t is_rom_writable(void)
{
  uint8_t byte0 = z80_bpeek(0);

  z80_bpoke( 0, ~byte0 );
  if( z80_bpeek(0) == byte0 )
    return 0;

  z80_bpoke( 0, byte0 );

  return 1;
}
