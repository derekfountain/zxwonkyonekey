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

#ifndef __LEVELS_H
#define __LEVELS_H

#include <stdint.h>

typedef struct _level_data
{
  void    (*draw_func)(void);

  uint8_t start_x;
  uint8_t start_y;

  uint8_t background_att;
  uint8_t solid_att;
  uint8_t jumper_att;
  uint8_t killer_att;
  uint8_t finish_att;
} LEVEL_DATA;

#define START_POINT(x,y) x,y

#define LEVEL_ATT_DATA(tag0,val0,tag1,val1,tag2,val2,tag3,val3,tag4,val4) val0,val1,val2,val3,val4

typedef struct _sp1_print_code
{
  uint8_t* bytes;
  uint8_t  length;
} SP1_PRINT_CODE;

#if 0
typedef enum _sp1_print_code_names
{
  SP1_PRINT_INK_BLACK=0,
  SP1_PRINT_INK_BLUE,
  SP1_PRINT_INK_RED,
  SP1_PRINT_INK_MAGENTA,
  SP1_PRINT_INK_GREEN,
  SP1_PRINT_INK_CYAN,
  SP1_PRINT_INK_YELLOW,
  SP1_PRINT_INK_WHITE,
} SP1_PRINT_CODE_NAMES;

SP1_PRINT_CODE sp1_print_code[] = {
  {"\x10\x00", 2},
  {"\x10\x01", 2},
  {"\x10\x02", 2},
  {"\x10\x03", 2},
  {"\x10\x04", 2},
  {"\x10\x05", 2},
  {"\x10\x06", 2},
  {"\x10\x07", 2},
};
#endif

LEVEL_DATA* get_level_data( uint8_t level );

#endif
