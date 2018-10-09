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

#ifndef __SLOWDOWN_PILL_H
#define __SLOWDOWN_PILL_H

/*
 * Slowdown point, stored as an x,y pixel location
 */
typedef struct _slowdown_definition
{
  uint8_t x;
  uint8_t y;

  struct sp1_ss*   sprite;

} SLOWDOWN_DEFINITION;


void create_slowdown_pill( SLOWDOWN_DEFINITION* slowdown );

#endif