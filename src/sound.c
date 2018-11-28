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

static uint8_t music_notes[] = {

  128,128,114,114,102,102,96,96,86,86,102,102,86,86,86,86,81,81,96,96,81,81,81,81,86,86,102,102,86,86,86,86,
  128,128,114,114,102,102,96,96,86,86,102,102,86,86,86,86,81,81,96,96,81,81,81,81,86,86,86,86,86,86,86,86,
  128,128,114,114,102,102,96,96,86,86,102,102,86,86,86,86,81,81,96,96,81,81,81,81,86,86,102,102,86,86,86,86,
  128,128,114,114,102,102,96,96,86,86,102,102,86,86,64,64,86,86,102,102,128,128,102,102,86,86,86,86,86,86,86,86,

  /*
    128,114,102,96,86,102,86,86,81,96,81,81,86,102,86,86,
    128,114,102,96,86,102,86,86,81,96,81,81,86,86,86,86,
    128,114,102,96,86,102,86,86,81,96,81,81,86,102,86,86,
    128,114,102,96,86,102,86,64,86,102,128,102,86,86,86,86,
  */
};

#define MUSIC_NUM_NOTES (sizeof(music_notes)/sizeof(music_notes[0]))

static uint16_t music_current_note_index = 0;

extern void play_mm_note( uint8_t* pitch );

void play_next_note( void )
{
  uint8_t current_note = music_notes[music_current_note_index++];
  play_mm_note( &current_note );

  if( music_current_note_index == MUSIC_NUM_NOTES )
    while(1);
  //    music_current_note_index = 0;
}
