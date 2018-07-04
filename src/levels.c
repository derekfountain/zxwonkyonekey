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

#include <arch/zx.h>
#include <arch/zx/sp1.h>

#include "levels.h"

void level1(void)
{
  uint8_t i;

  /*
   * I currently have no idea how I'm going to do this.
   */

  /* Bottom row */
  sp1_PrintAt(22,0,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(22,31,PAPER_GREEN|INK_BLACK,' ');
  for(i=0;i<32;i++)
    sp1_PrintAt(23,i,PAPER_GREEN|INK_BLACK,' ');


  /* Central area, 2 jumps and a block */
  sp1_PrintAt(23,15,PAPER_RED|INK_BLACK,' ');
  sp1_PrintAt(23,17,PAPER_RED|INK_BLACK,' ');
  sp1_PrintAt(22,16,PAPER_GREEN|INK_BLACK,' ');


  for(i=9;i<14;i++)
    sp1_PrintAt(20,i,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(20,8,PAPER_RED|INK_BLACK,' ');


  /* Gap */
  sp1_PrintAt(13,19,PAPER_WHITE|INK_BLACK,' ');

  /* Killer */
  /* Trampoline */
  sp1_PrintAt(13,22,PAPER_RED|INK_BLACK,' ');
  sp1_PrintAt(13,23,PAPER_BLUE|INK_BLACK,' ');
  sp1_PrintAt(13,24,PAPER_BLUE|INK_BLACK,' ');
  sp1_PrintAt(13,25,PAPER_RED|INK_BLACK,' ');

}
