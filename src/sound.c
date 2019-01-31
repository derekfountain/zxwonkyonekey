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
#include <sound.h>
#include "key_action.h"
#include "int.h"
#include "runner.h"

/*
 * The low level music note player routine in background_music.asm was stolen
 * from Manic Miner and adapted to run from C. I looked at the music data from
 * Manic Miner and reverse engineered the notes the data plays from sheet music.
 * Each note lasts 8.8ms and each one is repeated, so in fact each note lasts
 * 17.6ms.
 */

#define _C     86, 86
#define _D     81, 81
#define _E     72, 72
#define _F     64, 64
#define _G     57, 57
#define _A     51, 51
#define _B     48, 48

#define _UpC   43, 43
#define _UpD   40, 40
#define _UpE   36, 36

static uint8_t music_notes[] = {
#if 0
  /*
   * I originally wanted this to play "Little Boxes", the note data for which is here:
   *  https://noobnotes.net/little-boxes-malvina-reynolds/
   * But the music player doesn't handle short notes, long notes, notes that run
   * together, notes that need spacing, or anything else. It sounds bloody awful!
   */
  _G, _A, _E, _C,  _G, _A, _E, _C,                         /* Little boxes on the hillside */
  _G, _A, _UpC, _UpC, _UpC, _UpD, _UpE, _UpD, _UpC, _UpC,  /* Little boxes made of ticky-tacky */
  _UpC, _A, _G, _G, _G, _A, _F, _F,                        /* Little boxes on the hillside */
  _F, _G, _E, _E, _E, _F, _D,                              /* Little boxes all the same. */

  _G, _A, _E, _C, _G, _A, _E, _C,                          /* There's a pink one and a green one */
  _G, _A, _UpC, _UpC, _UpC, _UpD, _UpE, _UpD, _UpC,        /* And a blue one and a yellow one */
  _UpC, _A, _G, _G, _G, _A, _F, _F, _F, _F,                /* And they're all made out of ticky-tacky */
  _F, _G, _E, _E, _E, _D, _C,                              /* And they all look just the same. */
#else
  /*
   * So I switched to "London Bridge":
   *  https://noobnotes.net/london-bridge-traditional/
   * It still sounds bloody awful, but not quite as bloody awful as "Little Boxes".
   */
  _G, _A, _G, _F, _E, _F, _G,  _G,
  _D, _E, _F, _F, _E, _F, _G,
  _G, _A, _G, _F, _E, _F, _G, _G,
  _D, _D, _G, _G, _E, _C, _C
#endif
};
#define MUSIC_NUM_NOTES (sizeof(music_notes)/sizeof(music_notes[0]))

static uint8_t  music_on    = 0;
static uint8_t  slow_note   = 0;
static uint8_t  effects_on  = 1;

/* Keep a track of which note is playing */
static uint16_t music_current_note_index = 0;

/* This is the low level speaker waggler, in ASM */
extern void play_note_raw( uint8_t* pitch );

void toggle_music( void )
{
  music_on = !music_on;
}

/*
 * Sounds are split into 4 cycles. A cycle is a 50th of a second, i.e. one
 * frame of the Spectrum's display. Each and every cycle does the usual game
 * stuff, but because sound is CPU intensive we can't play sounds every cycle,
 * and we can't play both a music note and a sound effect in one cycle.
 * So, the spare time in every 4th cycle is handed over to playing background
 * music. The spare time in every other 4th cycle is handed over to playing
 * sound effects. There are 2 other cycles where the spare time is currently
 * not used.
 */
#define BACKGROUND_MUSIC_CYCLE 0x000
#define UNUSED_CYCLE_1         0x001
#define SOUND_EFFECT_CYCLE     0x002
#define UNUSED_CYCLE_2         0x003

PROCESSING_FLAG play_bg_music_note( void* data, GAME_ACTION* output_action )
{
  (void)data;

  *output_action = NO_ACTION;

  /*
   * Play a note every 4 game cycles. i.e. the game runs at 50fps, a note of music
   * is played every 4 of those. Each note lasts 8.8ms so every 4th game cycle
   * needs to complete in 11ms to ensure a frame isn't dropped. This currently
   * isn't a problem.
   */
  if( music_on && ((GET_TICKER & 0x0003) == BACKGROUND_MUSIC_CYCLE) )
  {
    play_note_raw( &(music_notes[music_current_note_index]) );

    if( GET_RUNNER_SLOWDOWN )
    {
      slow_note = !slow_note;
      if( !slow_note )
        music_current_note_index++;
    }
    else
    {
      slow_note = 0;
      music_current_note_index++;
    }

    if( music_current_note_index == MUSIC_NUM_NOTES )
      music_current_note_index = 0;

    /*
     * The action of playing a note is already done, so this just causes
     * the main gameloop to trace the fact.
     */
    *output_action = MUSIC_NOTE;
  }

  return KEEP_PROCESSING;

}

/*
 * Sound effects use the beepfx engine from Shuru:
 *
 *   https://shiru.untergrund.net/software.shtml
 *
 * which has been ported into Z88DK.
 *
 * There are lots of good effects, and the editor works in Windows 7 so
 * creating more is possible. The problem is that if the effect lasts
 * longer than about 10ms the game skips a frame which is rather obvious
 * and effects game play. So until I get to doing multi frame sound effects
 * the effects used are the very short ones. :)
 *
 * Sound effects are put into a queue, which is currently of length 1. :)
 * It's still important though, because the effect can only be played during
 * the correct cycle which is something the play function takes into account.
 *
 * There's a immediate play function too which is used on the odd occasion
 * when sound needs to play immediately, like end of level or teleport.
 */
void* pending_sound = 0;
void queue_beepfx_sound( void* sound )
{
  pending_sound = sound;
}

void toggle_sound_effects( void )
{
  effects_on = !effects_on;
  if( pending_sound )
    pending_sound = 0;
}

PROCESSING_FLAG play_beepfx_sound( void* data, GAME_ACTION* output_action )
{
  (void)data;

  *output_action = NO_ACTION;

  if( effects_on && pending_sound && ((GET_TICKER & 0x0003) == SOUND_EFFECT_CYCLE) )
  {
    bit_beepfx(pending_sound);
    pending_sound = 0;
    *output_action = SOUND_EFFECT;
  }

  return KEEP_PROCESSING;
}

void play_beepfx_sound_immediate( void* sound )
{
  if( effects_on )
    bit_beepfx(sound);
}
