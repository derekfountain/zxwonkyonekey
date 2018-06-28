#ifndef __INT_H
#define __INT_H

#include <stdint.h>

void setup_int(void);

/*
 * Macro (quicker than a function) to return the current
 * ticker value. This is essentially a 50Hz counter.
 */
#define GET_TICKER ((uint16_t)ticker)

#endif
