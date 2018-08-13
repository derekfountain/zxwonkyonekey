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
#include <malloc.h>

#include "utils.h"
#include "levels.h"

/*
 * These are the UDGs, defined in assembler because
 * they're easier to see with binary representation.
 */
extern uint8_t grassh[8];
extern uint8_t jumper[8];
extern uint8_t finish[8];
extern uint8_t grassv[8];

/*
 * Maps, also in assembler because the strings require
 * '\0' bytes.
 */
extern uint8_t level0_map[];
extern uint8_t level1_map[];

/*
 * Tables of tiles required for each level. "UDGs".
 * Tile number 0 is end of list; assumes I'll never
 * redefine tile zero.
 */
TILE_DEFINITION level0_tiles[] = {
  {128, grassh},
  {129, jumper},
  {130, finish},
  {131, grassv},
  {0,   {0}   }
};

TILE_DEFINITION level1_tiles[] = {
  {128, grassh},
  {129, jumper},
  {130, finish},
  {131, grassv},
  {0,   {0}   }
};

LEVEL_DATA level_data[] = {

  { print_level_from_sp1_string,
    level0_map,
    START_POINT(128,0),    /* TODO Use this for real game START_POINT(0,135), */
    LEVEL_BORDER(INK_BLACK),
    START_FACING(RIGHT),
    NAMED_VALUES_4("Background", INK_BLACK|PAPER_WHITE,
                   "Solid",      INK_GREEN|PAPER_WHITE,
                   "Jumper",     INK_RED|PAPER_GREEN,
                   "Finish",     INK_YELLOW|PAPER_BLUE),
    &level0_tiles[0]
  },
  { print_level_from_sp1_string,
    level1_map,
    START_POINT(0,135),
    LEVEL_BORDER(INK_BLUE),
    START_FACING(RIGHT),
    NAMED_VALUES_4("Background", INK_MAGENTA|PAPER_BLACK,
                   "Solid",      INK_WHITE|PAPER_BLACK,
                   "Jumper",     INK_RED|PAPER_WHITE,
                   "Finish",     INK_YELLOW|PAPER_BLUE),
    &level1_tiles[0],
  },
};


LEVEL_DATA* get_level_data( uint8_t level )
{
  return &level_data[level];
}

/*
 * This is defined in main.c. Just share it for now.
 */
extern struct sp1_Rect full_screen;

/*
 * This "prints" a level using the comprehensive SP1 print function.
 * The level data draw_data value should be a pointer to the string.
 * The string itself will likely need to be defined in ASM because
 * it'll probably require embedded zeroes.
 *
 * The SP1PrintString function is 731 bytes, and I don't use most of
 * the facilities, so this is quite expensive. The level0 data is 214
 * bytes. There will be more efficient ways to do this. OTOH, this is
 * supposed to be an SP1 learning exercise, so I'm doing it the SP1 way.
 */
void print_level_from_sp1_string(LEVEL_DATA* level_data)
{
  TILE_DEFINITION* tile_ptr;
  struct sp1_pss print_control = { &full_screen, SP1_PSSFLAG_INVALIDATE,
                                   0, 0,
                                   0x00, level_data->background_att,
                                   0,
                                   0 };

  /* Reset screen, remove tiles and sprites, and reset to new colours */
  sp1_ClearRectInv( &full_screen, level_data->background_att, ' ', SP1_RFLAG_TILE|
                                                                   SP1_RFLAG_COLOUR|
                                                                   SP1_RFLAG_SPRITE );

  /* Loop over tile defintitions, redefining them for the level */
  tile_ptr = level_data->level_tiles;
  while( tile_ptr->tile_num != 0 )
  {
    sp1_TileEntry(tile_ptr->tile_num, tile_ptr->udg_data);
    tile_ptr += 1;
  }

  sp1_PrintString(&print_control, (uint8_t*)(level_data->draw_data));
}
