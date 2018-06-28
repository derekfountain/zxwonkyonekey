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
