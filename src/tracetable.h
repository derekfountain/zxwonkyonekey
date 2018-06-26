#ifndef __TRACETABLE_H
#define __TRACETABLE_H

/*
 * Maximum amount of memory to allocate to tracetables.
 * We use the ROM area, so 16K.
 */
#define MAX_TRACE_MEMORY ((uint16_t)16384)

/*
 * Find out if the ROM is writable. In some emulators it can be.
 * Answers 1 if it is, or 0 if not.
 */
uint8_t is_rom_writable(void);

#endif
