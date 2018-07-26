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


  /* Central area, 1 jump */
  sp1_PrintAt(23,17,PAPER_RED|INK_BLACK,' ');

  /* Step 1 */
  for(i=9;i<14;i++)
    sp1_PrintAt(21,i,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(21,8,PAPER_RED|INK_BLACK,' ');

  /* Hop */
  sp1_PrintAt(19,4,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(19,5,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(19,6,PAPER_RED|INK_BLACK,' ');

  /* Step 2 */  
  for(i=10;i<15;i++)
    sp1_PrintAt(18,i,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(18,15,PAPER_RED|INK_BLACK,' ');

  /* Single jump */
  sp1_PrintAt(16,19,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(16,20,PAPER_RED|INK_BLACK,' ');

  /* Step 3 */
  for(i=23;i<26;i++)
    sp1_PrintAt(14,i,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(14,22,PAPER_RED|INK_BLACK,' ');


  /* Rest 1 */
  for(i=10;i<20;i++)
    sp1_PrintAt(13,i,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(12,10,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(12,19,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(13,11,PAPER_RED|INK_BLACK,' ');


  /* Step 4 and 5 */
  for(i=5;i<8;i++)
    sp1_PrintAt(11,i,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(11,8,PAPER_RED|INK_BLACK,' ');
  sp1_PrintAt(11,9,PAPER_GREEN|INK_BLACK,' ');

  for(i=4;i<6;i++)
    sp1_PrintAt(9,i,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(9,6,PAPER_RED|INK_BLACK,' ');


  /* Step 6 (long with gap) */
  for(i=10;i<24;i++)
    sp1_PrintAt(7,i,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(7,21,PAPER_WHITE|INK_BLACK,' ');
  sp1_PrintAt(7,20,PAPER_RED|INK_BLACK,' ');
  sp1_PrintAt(7,22,PAPER_RED|INK_BLACK,' ');


  /* Step 7 */
  for(i=21;i<23;i++)
    sp1_PrintAt(10,i,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(10,23,PAPER_RED|INK_BLACK,' ');

  /* Step 8 */
  for(i=26;i<27;i++)
    sp1_PrintAt(8,i,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(8,27,PAPER_RED|INK_BLACK,' ');

  /* Side wall */
  sp1_PrintAt(3,31,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(4,31,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(5,31,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(6,31,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(7,31,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(7,30,PAPER_RED|INK_BLACK,' ');
  sp1_PrintAt(7,29,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(7,28,PAPER_GREEN|INK_BLACK,' ');

  /* Step 8, long with killer */
  sp1_PrintAt(5,26,PAPER_RED|INK_BLACK,' ');
  for(i=3;i<25;i++)
    sp1_PrintAt(4,i,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(4,6,PAPER_BLUE|INK_BLACK,' ');
  sp1_PrintAt(4,7,PAPER_BLUE|INK_BLACK,' ');
  sp1_PrintAt(4,8,PAPER_BLUE|INK_BLACK,' ');
  sp1_PrintAt(4,9,PAPER_RED|INK_BLACK,' ');


  /* Finish */
  for(i=0;i<3;i++)
    sp1_PrintAt(16,i,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(15,0,PAPER_YELLOW|INK_BLACK,' ');
}
