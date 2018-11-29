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
#include "key_action.h"
#include "int.h"

#if 0

ORG 0xCC00

    DEFC _F = 128
    DEFC _G = 114
    DEFC _A = 102
    DEFC _B =  96
    DEFC _C =  86
    DEFC _D =  80 ; ?
    DEFC _E =  76 ; ?

notes:
 	DEFB 128,114,102,96,86,102,86,86,81,96,81,81,86,102,86,86
 	;      F   G   A  B  C   A  CCCC #B ?G  ?BBB  C   A  CCCC
 	DEFB 128,114,102,96,86,102,86,86,81,96,81,81,86,86,86,86
 	DEFB 128,114,102,96,86,102,86,86,81,96,81,81,86,102,86,86
 	DEFB 128,114,102,96,86,102,86,64,86,102,128,102,86,86,86,86
    ;                              F

; 	DEFB _G,_G, _A, _G, _E         ; Lit-tle box-es
; 	DEFB _G,_G, _A, _G, _E         ; on the hill-side
; 	DEFB _G,_G, _G, _C, _C         ; Lit-tle box-es
;	DEFB _C,_C, _D, _E, _D, _C, _C ; ma-de of ticky tacky

;	DEFB _C,_C, _A, _G, _G         ; Lit-tle box-es
;	DEFB _G,_G, _A, _F, _F         ; Lit-tle box-es
;	DEFB _F,_F, _G, _E, _E         ; Lit-tle box-es
;	DEFB _E,_E, _F, 64, 64         ; All the same

;   DEFB _C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C

; 	DEFB 114,114,102, 114,102, 114,114,102, 86,86, 74,74,86,86, 102,102,102,102
; 	DEFB 114,114,102,102, 114,114,102,102, 114,114,102,102, 86,86,86,86, 74,74,86,86, 102,102,102,102
; 	DEFB 114,114,102,102, 114,114,102,102, 114,114,102,102, 86,86,86,86, 74,74,86,86, 102,102,102,102
; 	DEFB 114,114,102,102, 114,114,102,102, 114,114,102,102, 86,86,86,86, 74,74,86,86, 102,102,102,102
; 	DEFB 114,114,102,102, 114,114,102,102, 114,114,102,102, 86,86,86,86, 74,74,86,86, 102,102,102,102
; 	DEFB 114,102,114,102, 114,102,114,102, 114,102,86,86, 74,86,102,102
; 	DEFB 114,102,114,102, 114,102,114,102, 114,102,86,86, 74,86,102,102
; 	DEFB 114,102,114,102, 114,102,114,102, 114,102,86,86, 74,86,102,102
; 	DEFB 114,102,114,102, 114,102,114,102, 114,102,86,86, 74,86,102,102



#endif

static uint8_t music_notes[] = {
  /* Each note lasts 8.8ms and each one is repeated, so in fact each note lasts 17.6ms */
  128,128,114,114,102,102,96,96,86,86,102,102,86,86,86,86,81,81,96,96,81,81,81,81,86,86,102,102,86,86,86,86,
  128,128,114,114,102,102,96,96,86,86,102,102,86,86,86,86,81,81,96,96,81,81,81,81,86,86,86,86,86,86,86,86,
  128,128,114,114,102,102,96,96,86,86,102,102,86,86,86,86,81,81,96,96,81,81,81,81,86,86,102,102,86,86,86,86,
  128,128,114,114,102,102,96,96,86,86,102,102,86,86,64,64,86,86,102,102,128,128,102,102,86,86,86,86,86,86,86,86,
};
#define MUSIC_NUM_NOTES (sizeof(music_notes)/sizeof(music_notes[0]))

static uint8_t  music_on = 0;

/* Keep a track of which note is playing */
static uint16_t music_current_note_index = 0;

/* This is the low level speaker waggler, in ASM */
extern void play_note_raw( uint8_t* pitch );

void toggle_music( void )
{
  music_on = !music_on;
}

PROCESSING_FLAG play_bg_music_note( void* data, GAME_ACTION* output_action )
{
  (void)data;

  /*
   * Play a note every 4 game cycles. i.e. the game runs at 50fps, a note of music
   * is played every 4 of those. Each note lasts 8.8ms so every 4th game cycle
   * needs to complete in 11ms to ensure a frame isn't dropped. This currently
   * isn't a problem.
   */
  if( music_on && ((GET_TICKER & 0x0003) == 0x003) )
  {
    play_note_raw( &(music_notes[music_current_note_index++]) );

    if( music_current_note_index == MUSIC_NUM_NOTES )
      music_current_note_index = 0;
  }

  *output_action = NO_ACTION;
  return KEEP_PROCESSING;

}

