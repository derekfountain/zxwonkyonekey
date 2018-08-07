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
#include <string.h>

#include "levels.h"

extern uint8_t grassh[];
#define GRASSH ('\x80')

extern uint8_t jumper[];
#define JUMPER ('\x81')

extern uint8_t finish[];
#define FINISH ('\x82')

extern uint8_t grassv[];
#define GRASSV ('\x83')

LEVEL_DATA level_data[] = {

  { level0,
    START_POINT(0,135),
    LEVEL_ATT_DATA("Background", INK_BLACK|PAPER_WHITE,
                   "Solid",      INK_GREEN|PAPER_WHITE,
                   "Jumper",     INK_RED|PAPER_GREEN,
                   "Killer",     INK_BLUE|PAPER_WHITE,
                   "Finish",     INK_YELLOW|PAPER_BLUE) },
};

LEVEL_DATA* get_level_data( uint8_t level )
{
  return &level_data[level];
}

extern struct sp1_Rect full_screen;

extern uint8_t  level1_map[];
extern uint8_t  level1_map_end[];

void level0(void)
{
  uint8_t i;
  uint8_t background_att  = level_data[0].background_att;
  uint8_t solid_att       = level_data[0].solid_att;
  uint8_t jumper_att      = level_data[0].jumper_att;
  uint8_t finish_att      = level_data[0].finish_att;

#define PRINT_BUFFER_SIZE 64
  uint8_t print_buffer[PRINT_BUFFER_SIZE];

  struct sp1_pss print_control = { &full_screen, SP1_PSSFLAG_INVALIDATE,
                                   0, 0,
                                   0x00, INK_WHITE|PAPER_WHITE,
                                   0,
                                   0 };

  sp1_TileEntry(128, grassh);
  sp1_TileEntry(129, jumper);
  sp1_TileEntry(130, finish);
  sp1_TileEntry(131, grassv);

  memcpy( print_buffer, &level1_map[0], (size_t)(level1_map_end-level1_map) );
  sp1_PrintString(&print_control, print_buffer);
 

  /* Central area, 1 jump */
  sp1_PrintAt(23,17,jumper_att,JUMPER);

  /* Step 1 */
  sp1_PrintAt(21,8,jumper_att,JUMPER);

  /* Hop */
  sp1_PrintAt(19,6,jumper_att,JUMPER);

  /* Step 2 */  
  sp1_PrintAt(18,15,jumper_att,JUMPER);

  /* Single jump */
  sp1_PrintAt(16,20,jumper_att,JUMPER);

  /* Step 3 */
//  for(i=23;i<26;i++)
//    sp1_PrintAt(14,i,solid_att,GRASSH);
  sp1_PrintAt(14,22,jumper_att,JUMPER);


  /* Rest 1 */
//  for(i=11;i<19;i++)
//    sp1_PrintAt(13,i,solid_att,GRASSH);
//  sp1_PrintAt(13,10,solid_att,GRASSV);
//  sp1_PrintAt(13,19,solid_att,GRASSV);
//  sp1_PrintAt(12,10,solid_att,GRASSH);
//  sp1_PrintAt(12,19,solid_att,GRASSH);
  sp1_PrintAt(13,11,jumper_att,JUMPER);


  /* Step 4 and 5 */
  for(i=5;i<8;i++)
    sp1_PrintAt(11,i,solid_att,GRASSH);
  sp1_PrintAt(11,8,jumper_att,JUMPER);
  sp1_PrintAt(11,9,solid_att,GRASSH);

  for(i=4;i<6;i++)
    sp1_PrintAt(9,i,solid_att,GRASSH);
  sp1_PrintAt(9,6,jumper_att,JUMPER);


  /* Step 6 (long with fall at end) */
  for(i=10;i<22;i++)
    sp1_PrintAt(7,i,solid_att,GRASSH);

  /* Step 7 */
  for(i=21;i<23;i++)
    sp1_PrintAt(10,i,solid_att,GRASSH);
  sp1_PrintAt(10,23,jumper_att,JUMPER);

  /* Step 8 */
  for(i=26;i<27;i++)
    sp1_PrintAt(8,i,solid_att,GRASSH);
  sp1_PrintAt(8,27,jumper_att,JUMPER);

  /* Side wall */
  sp1_PrintAt(3,31,solid_att,GRASSH);
  sp1_PrintAt(4,31,solid_att,GRASSV);
  sp1_PrintAt(5,31,solid_att,GRASSV);
  sp1_PrintAt(6,31,solid_att,GRASSV);
  sp1_PrintAt(7,31,solid_att,GRASSV);
  sp1_PrintAt(7,30,jumper_att,JUMPER);
  sp1_PrintAt(7,29,solid_att,GRASSH);
  sp1_PrintAt(7,28,solid_att,GRASSH);

  /* Isolated hop */
  sp1_PrintAt(5,26,jumper_att,JUMPER);

  /* Step 8 */
  for(i=3;i<9;i++)
    sp1_PrintAt(4,i,solid_att,GRASSH);
  for(i=11;i<25;i++)
    sp1_PrintAt(4,i,solid_att,GRASSH);
  sp1_PrintAt(3,5,solid_att,GRASSH);
  sp1_PrintAt(4,5,solid_att,GRASSV);
  sp1_PrintAt(4,10,jumper_att,JUMPER);


  /* Finish */
  for(i=0;i<3;i++)
    sp1_PrintAt(16,i,solid_att,GRASSH);
  sp1_PrintAt(15,0,finish_att,FINISH);
}
