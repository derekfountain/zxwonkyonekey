#ifndef __UTILS_H
#define __UTILS_H

/*
 * The modulo operator (%) is expensive with SDCC. It compiles in a CALL
 * to a function to do the work, and without hardware support that takes
 * quite a bit of effort. For cases where I just want to know if a value
 * is modulo 8 I can use this much faster macro instead.
 */ 
#define MODULO8(val)    ((val)&0x07)

/* These should be in zx.h, surely? */
#define ATTR_MASK_INK   ((uint8_t)0x03)
#define ATTR_MASK_PAPER ((uint8_t)0x38)

#endif
