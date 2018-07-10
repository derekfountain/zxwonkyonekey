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

#ifndef __ACTION_H
#define __ACTION_H

typedef enum _processing_flag
{
  KEEP_PROCESSING,
  STOP_PROCESSING,
}
PROCESSING_FLAG;

typedef enum _game_action
{
  NO_ACTION,
  MOVE_RIGHT,
  MOVE_LEFT,
  MOVE_UP,
  MOVE_DOWN,
  TOGGLE_DIRECTION,
  JUMP,
  DIE,
  FINISH,
}
GAME_ACTION;

typedef struct _loop_action
{
  PROCESSING_FLAG (*test_action)(void* input_data, GAME_ACTION* output_action); //, void* output_data);
}
LOOP_ACTION;

#endif
