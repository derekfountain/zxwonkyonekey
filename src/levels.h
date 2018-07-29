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

LEVEL_DATA* get_level_data( uint8_t level );

#endif
