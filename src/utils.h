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

/*
 * Macros to allow named contants. Just throw away every other value.
 */
#define NAMED_VALUES_1(name1,val1)                                               val1
#define NAMED_VALUES_2(name1,val1,name2,val2)                                    val1,val2
#define NAMED_VALUES_3(name1,val1,name2,val2,name3,val3)                         val1,val2,val3
#define NAMED_VALUES_4(name1,val1,name2,val2,name3,val3,name4,val4)              val1,val2,val3,val4
#define NAMED_VALUES_5(name1,val1,name2,val2,name3,val3,name4,val4,name5,val5)   val1,val2,val3,val4,val5

#endif
