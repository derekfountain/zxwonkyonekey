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
#include "slowdown_pill.h"
#include "teleporter.h"
#include "door.h"
#include "levels.h"

/*
 * These are the UDGs, defined in assembler because
 * they're easier to see with binary representation.
 */
extern uint8_t blank[8];
extern uint8_t grassh[8];
extern uint8_t jumper[8];
extern uint8_t platform1[8];
extern uint8_t platform1v[8];
extern uint8_t block_platform1[8];
extern uint8_t block_platform2[8];
extern uint8_t finish[8];
extern uint8_t grassv[8];
extern uint8_t teleporter[8];
extern uint8_t door_key[8];

/*
 * Maps, also in assembler because the strings require
 * '\0' bytes.
 */
extern uint8_t level0_map[];
extern uint8_t level1_map[];
extern uint8_t level2_map[];

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
  {128, platform1},
  {129, jumper},
  {130, finish},
  {131, platform1v},
  {132, teleporter},
  {0,   {0}   }
};

TILE_DEFINITION level2_tiles[] = {
  {128, block_platform1},
  {129, jumper},
  {130, finish},
  {131, block_platform2},
  {132, teleporter},
  {133, door_key},
  {255, blank},
  {0,   {0}   }
};

TELEPORTER_DEFINITION level1_teleporters[] = {
  {  0*8,  1*8,  0,  1,
    22*8, 30*8, 22, 30,
    NAMED_VALUES_1("Change direction", 0),
    INK_GREEN,
    INK_MAGENTA,
  },
  {  0*8, 10*8,  0, 10,
    22*8, 10*8, 22, 10,
    NAMED_VALUES_1("Change direction", 1),
    INK_GREEN,
    INK_MAGENTA,
  },
  { 14*8, 20*8, 14, 20,
     3*8,  0*8,  3,  0,
    NAMED_VALUES_1("Change direction", 0),
    INK_GREEN,
    INK_MAGENTA,
  },
  { 11*8, 28*8, 11, 28,
     6*8,  0*8,  6,  0,
    NAMED_VALUES_1("Change direction", 1),
    INK_GREEN,
    INK_MAGENTA,
  },
  {  4*8,  8*8,  4,  8,
     2*8, 30*8,  2, 30,
    NAMED_VALUES_1("Change direction", 0),
    INK_GREEN,
    INK_MAGENTA,
  },
  {  2*8, 23*8,  2, 23,
    16*8,  6*8,  16, 6,
    NAMED_VALUES_1("Change direction", 0),
    INK_GREEN,
    INK_MAGENTA,
  },
  {0}
};

SLOWDOWN level0_slowdowns[] = {
  { {INITIALISE_COLLECTABLE("Sprite x", 184, "Sprite y", 176,
                            "Centre x", 188, "Centre y", 180,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(12) },
  { {INITIALISE_COLLECTABLE("Sprite x",  24, "Sprite y",  64,
                            "Centre x",  28, "Centre y",  68,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(15) },
  { {INITIALISE_COLLECTABLE("Sprite x", 208, "Sprite y", 104,
                            "Centre x", 210, "Centre y", 108,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(15) },
  { {INITIALISE_COLLECTABLE("Sprite x",   0, "Sprite y",   0,
                            "Centre x",   0, "Centre y",   0,
                            0,
                            0)}},
};

SLOWDOWN level1_slowdowns[] = {
  { {INITIALISE_COLLECTABLE("Sprite x", 180, "Sprite y", 128,
                            "Centre x", 184, "Centre y", 132,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(15)  },
  { {INITIALISE_COLLECTABLE("Sprite x", 240, "Sprite y", 88,
                            "Centre x", 244, "Centre y", 92,
                            slowdown_collected,
                            slowdown_timeup)},  SLOWDOWN_SECS(12)  },
  { {INITIALISE_COLLECTABLE("Sprite x",   0, "Sprite y",   0,
                            "Centre x",   0, "Centre y",   0,
                            0,
                            0)}},
};

SLOWDOWN level2_slowdowns[] = {
  { {INITIALISE_COLLECTABLE("Sprite x", 14*8,   "Sprite y", 20*8,
                            "Centre x", 14*8+4, "Centre y", 20*8+4,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(12)  },
  { {INITIALISE_COLLECTABLE("Sprite x", 1*8,    "Sprite y", 11*8,
                            "Centre x", 1*8+4,  "Centre y", 11*8+4,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(15)  },
  { {INITIALISE_COLLECTABLE("Sprite x", 30*8,   "Sprite y", 20*8,
                            "Centre x", 30*8+4, "Centre y", 20*8+4,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(15)  },
  { {INITIALISE_COLLECTABLE("Sprite x", 23*8,   "Sprite y",  9*8,
                            "Centre x", 23*8+4, "Centre y",  9*8+4,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(15)  },
  { {INITIALISE_COLLECTABLE("Sprite x",   0, "Sprite y",   0,
                            "Centre x",   0, "Centre y",   0,
                            0,
                            0)}},
};

DOOR level2_doors[] = {
  { {INITIALISE_COLLECTABLE("Key x cell",       8,  "Key y cell",        4,
                            "Centre x px", (8*8+4), "Centre y px", (4*8+4),
                            door_key_collected,
                            door_open_timeup)},
    INITIALISE_DOOR_LOCATION(17,22),
    NAMED_VALUES_1("Door ink",     INK_MAGENTA),
    NAMED_VALUES_1("Key ink",        INK_BLACK),
    NAMED_VALUES_1("Key paper",      INK_WHITE),
    NAMED_VALUES_1("Key tile",             133),
    NAMED_VALUES_1("Door open secs",        10),
    NAMED_VALUES_1("Start open secs",        2),
  },
  { {INITIALISE_COLLECTABLE("Key x cell",       5,  "Key y cell",       22,
                            "Centre x px", (5*8+4), "Centre y px", (22*8+4),
                            door_key_collected,
                            door_open_timeup)},
    INITIALISE_DOOR_LOCATION(22,22),
    NAMED_VALUES_1("Door ink",        INK_BLUE),
    NAMED_VALUES_1("Key ink",        INK_BLACK),
    NAMED_VALUES_1("Key paper",      INK_WHITE),
    NAMED_VALUES_1("Key tile",             133),
    NAMED_VALUES_1("Door open secs",         5),
    NAMED_VALUES_1("Start open secs",        3),
  },
  { {INITIALISE_COLLECTABLE("Key x cell",       30,  "Key y cell",        4,
                            "Centre x px", (30*8+4), "Centre y px", (4*8+4),
                            door_key_collected,
                            door_open_timeup)},
    INITIALISE_DOOR_LOCATION(27,22),
    NAMED_VALUES_1("Door ink",       INK_GREEN),
    NAMED_VALUES_1("Key ink",        INK_BLACK),
    NAMED_VALUES_1("Key paper",      INK_WHITE),
    NAMED_VALUES_1("Key tile",             133),
    NAMED_VALUES_1("Door open secs",         6),
    NAMED_VALUES_1("Start open secs",        4),
  },
  { {INITIALISE_COLLECTABLE("Sprite x",   0, "Sprite y",   0,
                            "Centre x",   0, "Centre y",   0,
                            0,
                            0)}, 0 },
};

TELEPORTER_DEFINITION level2_teleporters[] = {
  { 15*8, 13*8, 15, 13,
    14*8, 22*8, 14, 22,
    NAMED_VALUES_1("Change direction", 1),
    INK_GREEN,
    INK_MAGENTA,
  },
  {  9*8, 14*8,  9, 14,
     4*8,  1*8,  4,  1,
    NAMED_VALUES_1("Change direction", 1),
    INK_GREEN,
    INK_MAGENTA,
  },
  {  13*8, 24*8,  13, 24,
     9*8,  30*8,  9,  30,
    NAMED_VALUES_1("Change direction", 0),
    INK_GREEN,
    INK_MAGENTA,
  },
  {0}
};

LEVEL_DATA level_data[] = {

  /***
   *      _                _    __
   *     | |   _____ _____| |  /  \
   *     | |__/ -_\ V / -_| | | () |
   *     |____\___|\_/\___|_|  \__/
   *
   */
  { print_level_from_sp1_string,
    level0_map,
    teardown_level,
    START_POINT(0,135),
    LEVEL_BORDER(INK_BLACK),
    START_FACING(RIGHT),
    NAMED_VALUES_5("Background", INK_BLACK|PAPER_WHITE,
                   "Solid",      INK_GREEN|PAPER_WHITE,
                   "Jumper",     INK_RED|PAPER_GREEN,
                   "Teleporter", INK_BLACK|PAPER_WHITE,
                   "Finish",     INK_YELLOW|PAPER_BLUE),
    &level0_tiles[0],
    NULL,
    &level0_slowdowns[0],
    NULL,
    MAX_POINTS(12500),
    MAX_BONUS(5000),
    { NAMED_VALUES_5("Level score X", 19,
                     "Level score Y", 18,
                     "Bonus score X", 19,
                     "Bonus score Y", 19,
		     "Scores att",    INK_BLUE|PAPER_WHITE) }
  },

  /***
   *      _                _   _
   *     | |   _____ _____| | / |
   *     | |__/ -_\ V / -_| | | |
   *     |____\___|\_/\___|_| |_|
   *
   */
  { print_level_from_sp1_string,
    level1_map,
    teardown_level,
    START_POINT(0,155),
    LEVEL_BORDER(INK_BLUE),
    START_FACING(RIGHT),
    NAMED_VALUES_5("Background", INK_MAGENTA|PAPER_BLACK,
                   "Solid",      INK_WHITE|PAPER_BLACK,
                   "Jumper",     INK_RED|PAPER_BLACK,
                   "Teleporter", INK_GREEN|PAPER_MAGENTA,
                   "Finish",     INK_YELLOW|PAPER_BLUE),
    &level1_tiles[0],
    &level1_teleporters[0],
    &level1_slowdowns[0],
    NULL,
    MAX_POINTS(15000),
    MAX_BONUS(8000),
    { NAMED_VALUES_5("Level score X", 14,
                     "Level score Y", 19,
                     "Bonus score X", 14,
                     "Bonus score Y", 20,
		     "Scores att",    INK_YELLOW|PAPER_BLACK) }
  },

  /***
   *      _                _   ___
   *     | |   _____ _____| | |_  )
   *     | |__/ -_\ V / -_| |  / /
   *     |____\___|\_/\___|_| /___|
   *
   */
  { print_level_from_sp1_string,
    level2_map,
    teardown_level,
    /*START_POINT(0,155),*/
    /*START_POINT(48,120),*/
    /*START_POINT(8,16),*/
    /*START_POINT(24*8,21*8),*/
    START_POINT(26*8,13*8),
    LEVEL_BORDER(INK_BLACK),
    START_FACING(RIGHT),
    NAMED_VALUES_5("Background", INK_BLACK|PAPER_WHITE,
                   "Solid",      INK_RED|PAPER_WHITE,
                   "Jumper",     INK_YELLOW|PAPER_RED,
                   "Teleporter", INK_GREEN|PAPER_MAGENTA,
                   "Finish",     INK_YELLOW|PAPER_BLUE),
    &level2_tiles[0],
    &level2_teleporters[0],
    level2_slowdowns,
    &level2_doors[0],
    MAX_POINTS(20000),
    MAX_BONUS(12000),
    { NAMED_VALUES_5("Level score X",  0,
                     "Level score Y",  0,
                     "Bonus score X",  0,
                     "Bonus score Y",  1,
		     "Scores att",    INK_BLUE|PAPER_WHITE) }
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

/*
 * Keep print control structure global so the compiler can allocate it.
 * Also, this can be used from any other part of the level display
 * which needs SP1 print control (after the level printing code which
 * initialises it has run).
 */
struct sp1_pss level_print_control = { &full_screen, SP1_PSSFLAG_INVALIDATE,
                                       0, 0,
                                       0x00, 0,
                                       0,
                                       0 };
void print_level_from_sp1_string(LEVEL_DATA* level_data)
{
  TILE_DEFINITION* tile_ptr;

  level_print_control.attr = level_data->background_att;

  /* Reset screen, remove tiles and sprites, and reset to new colours */
  sp1_ClearRectInv( &full_screen, level_data->background_att, ' ', SP1_RFLAG_TILE|
                                                                   SP1_RFLAG_COLOUR|
                                                                   SP1_RFLAG_SPRITE );

  /* Loop over tile definitions, redefining them for the level */
  tile_ptr = level_data->level_tiles;
  while( tile_ptr->tile_num != 0 )
  {
    sp1_TileEntry(tile_ptr->tile_num, tile_ptr->udg_data);
    tile_ptr += 1;
  }

  /* Print the string from the levels map data */
  sp1_PrintString(&level_print_control, (uint8_t*)(level_data->draw_data));

  /*
   * If the level has teleporters they are filled in here. These could be
   * done in the level map string in the ASM code, but they are required
   * in the C code as well so their cells can be vaildated. The duplication
   * was confusing, so I moved the code to deal with them here. It's less
   * efficient space-wise but makes things easier when putting the level
   * together.
   */
  if( level_data->teleporters )
  {
    TELEPORTER_DEFINITION* teleporter = level_data->teleporters;

    uint8_t print_string[9];

    print_string[0] = '\x10'; /* Ink <n> */
    print_string[2] = '\x11'; /* Paper <n> */

    print_string[4] = '\x16'; /* AT <x>,<y> */

    print_string[8] = '\0';

    while( teleporter->end_1_x || teleporter->end_1_y )
    {
      /* TODO Pull this out into a teleporter.c file */
      
      print_string[1] = teleporter->ink;
      print_string[3] = teleporter->paper;

      print_string[5] = teleporter->end_1_y_cell;
      print_string[6] = teleporter->end_1_x_cell;
      print_string[7] = '\x84'; /* FIXME Hardcoded teleporter UDG number */

      sp1_PrintString(&level_print_control, print_string);

      /* Change the other end and print again */
      print_string[5] = teleporter->end_2_y_cell;
      print_string[6] = teleporter->end_2_x_cell;

      sp1_PrintString(&level_print_control, print_string);
    
      teleporter++;
    }
  }

  if( level_data->slowdowns )
  {
    SLOWDOWN* slowdown = level_data->slowdowns;

    while( IS_VALID_COLLECTABLE(slowdown->collectable) )
    {
      create_slowdown_pill( slowdown );
      slowdown++;
    }
  }

  if( level_data->doors )
  {
    DOOR* door = level_data->doors;

    while( IS_VALID_COLLECTABLE(door->collectable) )
    {
      create_door( door );
      door++;
    }
  }

}


void teardown_level(LEVEL_DATA* level_data)
{
  /* Reclaim slowdown pill memory (SP1 structs, etc)*/
  if( level_data->slowdowns )
  {
    SLOWDOWN* slowdown = level_data->slowdowns;

    while( IS_VALID_COLLECTABLE(slowdown->collectable) )
    {
      destroy_slowdown_pill( slowdown );
      slowdown++;
    }
  }

  if( level_data->doors )
  {
    DOOR* door = level_data->doors;

    while( IS_VALID_COLLECTABLE(door->collectable) )
    {
      destroy_door( door );
      door++;
    }
  }

}
