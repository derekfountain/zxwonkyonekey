#include <stdint.h>
#include <z80.h>

#include "tracetable.h"

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

uint8_t is_rom_writable(void)
{
  uint8_t byte0 = z80_bpeek(0);

  TRACE_ENTRY t;

  t.i8 = 0x55;
  t.i16 = 0xAAAA;
  t.ptr = (void*)add_trace;

  add_trace( &t );
  while(1);

  z80_bpoke( 0, ~byte0 );
  if( z80_bpeek(0) == byte0 )
    return 0;

  z80_bpoke( 0, byte0 );
  return 1;
}
