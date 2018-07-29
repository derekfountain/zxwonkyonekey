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

LEVEL_DATA level_data[] = {

  { level0,
    START_POINT(0,135),
    LEVEL_ATT_DATA("Background", PAPER_WHITE,
                   "Solid",      PAPER_GREEN,
                   "Jumper",     PAPER_RED,
                   "Killer",     PAPER_BLUE,
                   "Finish",     PAPER_YELLOW) },
};

LEVEL_DATA* get_level_data( uint8_t level )
{
  return &level_data[level];
}

void level0(void)
{
  uint8_t i;
  uint8_t background_att  = level_data[0].background_att;
  uint8_t solid_att       = level_data[0].solid_att;
  uint8_t jumper_att      = level_data[0].jumper_att;
  uint8_t finish_att      = level_data[0].finish_att;

  /*
   * I currently have no idea how I'm going to do this.
   */

  /* Bottom row */
  sp1_PrintAt(22,0,solid_att,' ');
  sp1_PrintAt(22,31,solid_att,' ');
  for(i=0;i<32;i++)
    sp1_PrintAt(23,i,solid_att,' ');


  /* Central area, 1 jump */
  sp1_PrintAt(23,17,jumper_att,' ');

  /* Step 1 */
  for(i=9;i<14;i++)
    sp1_PrintAt(21,i,solid_att,' ');
  sp1_PrintAt(21,8,jumper_att,' ');

  /* Hop */
  sp1_PrintAt(19,4,solid_att,' ');
  sp1_PrintAt(19,5,solid_att,' ');
  sp1_PrintAt(19,6,jumper_att,' ');

  /* Step 2 */  
  for(i=10;i<15;i++)
    sp1_PrintAt(18,i,solid_att,' ');
  sp1_PrintAt(18,15,jumper_att,' ');

  /* Single jump */
  sp1_PrintAt(16,19,solid_att,' ');
  sp1_PrintAt(16,20,jumper_att,' ');

  /* Step 3 */
  for(i=23;i<26;i++)
    sp1_PrintAt(14,i,solid_att,' ');
  sp1_PrintAt(14,22,jumper_att,' ');


  /* Rest 1 */
  for(i=10;i<20;i++)
    sp1_PrintAt(13,i,solid_att,' ');
  sp1_PrintAt(12,10,solid_att,' ');
  sp1_PrintAt(12,19,solid_att,' ');
  sp1_PrintAt(13,11,jumper_att,' ');


  /* Step 4 and 5 */
  for(i=5;i<8;i++)
    sp1_PrintAt(11,i,solid_att,' ');
  sp1_PrintAt(11,8,jumper_att,' ');
  sp1_PrintAt(11,9,solid_att,' ');

  for(i=4;i<6;i++)
    sp1_PrintAt(9,i,solid_att,' ');
  sp1_PrintAt(9,6,jumper_att,' ');


  /* Step 6 (long with gap) */
  for(i=10;i<24;i++)
    sp1_PrintAt(7,i,solid_att,' ');
  sp1_PrintAt(7,21,background_att,' ');


  /* Step 7 */
  for(i=21;i<23;i++)
    sp1_PrintAt(10,i,solid_att,' ');
  sp1_PrintAt(10,23,jumper_att,' ');

  /* Step 8 */
  for(i=26;i<27;i++)
    sp1_PrintAt(8,i,solid_att,' ');
  sp1_PrintAt(8,27,jumper_att,' ');

  /* Side wall */
  sp1_PrintAt(3,31,solid_att,' ');
  sp1_PrintAt(4,31,solid_att,' ');
  sp1_PrintAt(5,31,solid_att,' ');
  sp1_PrintAt(6,31,solid_att,' ');
  sp1_PrintAt(7,31,solid_att,' ');
  sp1_PrintAt(7,30,jumper_att,' ');
  sp1_PrintAt(7,29,solid_att,' ');
  sp1_PrintAt(7,28,solid_att,' ');

  /* Step 8 */
  sp1_PrintAt(5,26,jumper_att,' ');
  for(i=3;i<25;i++)
    sp1_PrintAt(4,i,solid_att,' ');
  sp1_PrintAt(3,5,solid_att,' ');
  sp1_PrintAt(4,9,background_att,' ');
  sp1_PrintAt(4,10,jumper_att,' ');


  /* Finish */
  for(i=0;i<3;i++)
    sp1_PrintAt(16,i,solid_att,' ');
  sp1_PrintAt(15,0,finish_att,' ');
}
