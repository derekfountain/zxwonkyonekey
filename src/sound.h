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

#ifndef __SOUND_H
#define __SOUND_H

void toggle_music( void );
PROCESSING_FLAG play_bg_music_note( void* data, GAME_ACTION* output_action );

void toggle_sound_effects( void );
void queue_beepfx_sound( void* sound );
PROCESSING_FLAG play_beepfx_sound( void* data, GAME_ACTION* output_action );
void play_beepfx_sound_immediate( void* sound );

#endif
