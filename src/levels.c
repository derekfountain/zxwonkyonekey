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

extern uint8_t grassh[];
extern uint8_t jumper[];
extern uint8_t finish[];
extern uint8_t grassv[];

extern uint8_t level0_map[];
extern uint8_t level1_map[];

LEVEL_DATA level_data[] = {

  { print_level_from_sp1_string,
    level0_map,
    RUNNER_ATT(INK_BLACK|PAPER_WHITE),
    START_POINT(0,135),
    LEVEL_BORDER(INK_BLACK),
    START_FACING(RIGHT),
    NAMED_VALUES_5("Background", INK_BLACK|PAPER_WHITE,
                   "Solid",      INK_GREEN|PAPER_WHITE,
                   "Jumper",     INK_RED|PAPER_GREEN,
                   "Killer",     INK_BLUE|PAPER_WHITE,
                   "Finish",     INK_YELLOW|PAPER_BLUE) },
  { print_level_from_sp1_string,
    level1_map,
    RUNNER_ATT(INK_MAGENTA|PAPER_BLACK),
    START_POINT(0,135),
    LEVEL_BORDER(INK_BLUE),
    START_FACING(RIGHT),
    NAMED_VALUES_5("Background", INK_MAGENTA|PAPER_BLACK,
                   "Solid",      INK_MAGENTA|PAPER_BLACK,
                   "Jumper",     INK_RED|PAPER_WHITE,
                   "Killer",     INK_BLUE|PAPER_WHITE,
                   "Finish",     INK_YELLOW|PAPER_BLUE) },
};

/*
Something not right here. The runner att is ignored, and the solid att is used
instead. But if they differ the collision detection doesn't work.
Not sure what's going on. Review use of colour.
 */

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
  struct sp1_pss print_control = { &full_screen, SP1_PSSFLAG_INVALIDATE,
                                   0, 0,
                                   0x00, level_data->background_att,
                                   0,
                                   0 };

  /* Reset screen, remove tiles and sprites, and reset to new colours */
  sp1_ClearRectInv( &full_screen, level_data->background_att, ' ', SP1_RFLAG_TILE|SP1_RFLAG_COLOUR|SP1_RFLAG_SPRITE );

  /* TODO These should be in the level data so they can vary from level to level */
  sp1_TileEntry(128, grassh);
  sp1_TileEntry(129, jumper);
  sp1_TileEntry(130, finish);
  sp1_TileEntry(131, grassv);

  sp1_PrintString(&print_control, (uint8_t*)(level_data->draw_data));
}
