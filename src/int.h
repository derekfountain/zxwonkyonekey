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

#ifndef __INT_H
#define __INT_H

#include <stdint.h>
#include <intrinsic.h>

extern uint16_t ticker;

void setup_int(void);

/*
 * Macro (quicker than a function) to return the current
 * ticker value. This is essentially a 50Hz counter.
 *
 * It's 16 bits so will compile to separate load instructions
 * on the Z80, hence it needs the atomic wrapper.
 */
#define GET_TICKER ((uint16_t)intrinsic_load16(_ticker))

#endif
