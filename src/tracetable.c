#include <stddef.h>
#include <stdint.h>
#include <z80.h>

#include "tracetable.h"

/*
trace entry is a client thing. the structure of the entry, plus
the tracetable, plus the index of the slot for the next entry,
all need to go into the client code.
use macros to build all this in a generic way?
 */
typedef struct _trace_entry
{
  uint8_t i8;
  uint16_t i16;
  void* ptr;
} TRACE_ENTRY;


static TRACE_ENTRY* tracetable_head = 0;

void add_trace( TRACE_ENTRY* trace )
{
  TRACE_ENTRY* trace_addr = tracetable_head;
  
  trace_addr->i8 = trace->i8;
  trace_addr->i16 = trace->i16;
  trace_addr->ptr = trace->ptr;

  tracetable_head += sizeof(TRACE_ENTRY);
}

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
  return 1;
}
