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

#include <stdint.h>
#include <arch/zx/sp1.h>

#include "slowdown_pill.h"

extern uint8_t slowdown_pill_f1[];
extern uint8_t slowdown_pill_f2[];
extern uint8_t slowdown_pill_f3[];
extern uint8_t slowdown_pill_f4[];

/*
 * This is defined in main.c. Just share it for now.
 */
extern struct sp1_Rect full_screen;

void create_slowdown_pill( SLOWDOWN_DEFINITION* slowdown )
{
  slowdown->sprite = sp1_CreateSpr(SP1_DRAW_LOAD1LB, SP1_TYPE_1BYTE, 2,
                                   (int)slowdown_pill_f1, 0);
  sp1_AddColSpr(slowdown->sprite, SP1_DRAW_LOAD1RB, SP1_TYPE_1BYTE, 0, 0);

  sp1_MoveSprPix(slowdown->sprite, &full_screen, 0, slowdown->x, slowdown->y);
}
