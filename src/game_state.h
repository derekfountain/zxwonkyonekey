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

#ifndef __GAME_STATE_H
#define __GAME_STATE_H

#include <stdint.h>

#include "levels.h"

/*
 * This structure defines the state of the game.
 *
 * There's probably level information, number of lives left,
 * and goodness knows what else to be added here.
 */
typedef struct _game_state
{
  uint8_t     key_pressed;
  uint8_t     key_processed;

  LEVEL_DATA* current_level;
} GAME_STATE;

#endif
