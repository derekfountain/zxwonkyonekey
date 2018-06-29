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
 * Still not quite sure how this is going to hang together.
 */

static uint8_t* tracetable_head = (void*)0;

void* allocate_tracetable( size_t size )
{
  void* allocated_block;

  if( (!is_rom_writable()) || (size_t)tracetable_head + size > MAX_TRACE_MEMORY )
    return NULL;

  allocated_block = tracetable_head;
  tracetable_head += size;

  return allocated_block;
}

/*
 * TODO This needs to go into a utility.c file
 */
uint8_t is_rom_writable(void)
{
  uint8_t byte0 = z80_bpeek(0);

  z80_bpoke( 0, ~byte0 );
  if( z80_bpeek(0) == byte0 )
    return 0;

  z80_bpoke( 0, byte0 );

  /* TODO Move this. It can't be here in case this routine ends up in a fast loop. */
  memset(0, 0, 0x4000);

  return 1;
}
