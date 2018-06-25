#ifndef __TRACETABLE_H
#define __TRACETABLE_H

/*
 * Find out if the ROM is writable. In some emulators it can be.
 * Answers 1 if it is, or 0 if not.
 */
uint8_t is_rom_writable(void);

#endif
