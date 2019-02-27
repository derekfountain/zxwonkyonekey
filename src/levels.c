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
#include "graphics.h"

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
extern uint8_t block_platform3[8];
extern uint8_t block_platform4[8];
extern uint8_t block_platform5[8];
extern uint8_t finish[8];
extern uint8_t grassv[8];
extern uint8_t teleporter[8];
extern uint8_t door_key[8];
extern uint8_t score_slider_left[8];
extern uint8_t score_slider_right[8];
extern uint8_t score_slider_centre[8];

/*
 * Maps, also in assembler because the strings require
 * '\0' bytes.
 */
extern uint8_t level_intro_map[];
extern uint8_t level0_map[];
extern uint8_t level1_map[];
extern uint8_t level2_map[];
extern uint8_t level3_map[];
extern uint8_t level4_map[];


/***
 *      _______ _ _
 *     |__   __(_| |
 *        | |   _| | ___ ___
 *        | |  | | |/ _ / __|
 *        | |  | | |  __\__ \
 *        |_|  |_|_|\___|___/
 *
 *
 */

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
  {140, score_slider_left},
  {141, score_slider_right},
  {142, score_slider_centre},
  {0,   {0}   }
};

TILE_DEFINITION level1_tiles[] = {
  {128, platform1},
  {129, jumper},
  {130, finish},
  {131, platform1v},
  {132, teleporter},
  {140, score_slider_left},
  {141, score_slider_right},
  {142, score_slider_centre},
  {0,   {0}   }
};

TILE_DEFINITION level2_tiles[] = {
  {128, block_platform1},
  {129, jumper},
  {130, finish},
  {131, block_platform2},
  {132, teleporter},
  {133, door_key},
  {140, score_slider_left},
  {141, score_slider_right},
  {142, score_slider_centre},
  {255, blank},
  {0,   {0}   }
};

TILE_DEFINITION level3_tiles[] = {
  {128, block_platform4},
  {129, jumper},
  {130, finish},
  {132, teleporter},
  {133, door_key},
  {140, score_slider_left},
  {141, score_slider_right},
  {142, score_slider_centre},
  {255, blank},
  {0,   {0}   }
};

TILE_DEFINITION level4_tiles[] = {
  {128, block_platform5},
  {129, jumper},
  {130, finish},
  {131, block_platform5},
  {132, teleporter},
  {133, door_key},
  {140, score_slider_left},
  {141, score_slider_right},
  {142, score_slider_centre},
  {255, blank},
  {0,   {0}   }
};


/***
 *      _______   _                       _
 *     |__   __| | |                     | |
 *        | | ___| | ___ _ __   ___  _ __| |_ ___ _ __ ___
 *        | |/ _ | |/ _ | '_ \ / _ \| '__| __/ _ | '__/ __|
 *        | |  __| |  __| |_) | (_) | |  | ||  __| |  \__ \
 *        |_|\___|_|\___| .__/ \___/|_|   \__\___|_|  |___/
 *                      | |
 *                      |_|
 */

TELEPORTER_DEFINITION level1_teleporters[] = {
  {  0*8,  1*8,  0,  1,
    22*8, 30*8, 22, 30,
    NAMED_VALUES_1("Change direction", 0)
  },
  {  0*8, 10*8,  0, 10,
    22*8, 10*8, 22, 10,
    NAMED_VALUES_1("Change direction", 1)
  },
  { 14*8, 20*8, 14, 20,
     3*8,  0*8,  3,  0,
    NAMED_VALUES_1("Change direction", 0)
  },
  { 11*8, 28*8, 11, 28,
     6*8,  0*8,  6,  0,
    NAMED_VALUES_1("Change direction", 1)
  },
  {  4*8,  8*8,  4,  8,
     2*8, 30*8,  2, 30,
    NAMED_VALUES_1("Change direction", 0)
  },
  {  2*8, 23*8,  2, 23,
    16*8,  6*8,  16, 6,
    NAMED_VALUES_1("Change direction", 0)
  },
  {0}
};

TELEPORTER_DEFINITION level2_teleporters[] = {
  { 15*8, 13*8, 15, 13,
    14*8, 22*8, 14, 22,
    NAMED_VALUES_1("Change direction", 1)
  },
  {  9*8, 14*8,  9, 14,
     4*8,  1*8,  4,  1,
    NAMED_VALUES_1("Change direction", 0)
  },
  {  13*8, 24*8,  13, 24,
     9*8,  30*8,  9,  30,
    NAMED_VALUES_1("Change direction", 0)
  },
  {0}
};

TELEPORTER_DEFINITION level3_teleporters[] = {
  /* Bottom left up to top left */
  {  4*8,  1*8,  4,  1,
    22*8,  1*8, 22,  1,
    NAMED_VALUES_1("Change direction", 1)
  },
  /* Third platform up to central puzzle start */
  { 14*8,  7*8, 14,  7,
     6*8, 16*8,  6, 16,
    NAMED_VALUES_1("Change direction", 0)
  },
  /* Lower left decoy */
  { 10*8,  7*8, 10,  7,
    10*8, 25*8, 10, 25,
    NAMED_VALUES_1("Change direction", 1)
  },
  /* Upper left decoy */
  { 8*8,  7*8, 8,  7,
    12*8, 25*8, 12, 25,
    NAMED_VALUES_1("Change direction", 1)
  },
  /* Upper right passage */
  { 8*8, 25*8, 8, 25,
    2*8, 30*8, 2, 30,
    NAMED_VALUES_1("Change direction", 1)
  },
  {  9*8, 30*8,  9, 30,
    22*8, 27*8, 22, 27,
    NAMED_VALUES_1("Change direction", 0)
  },
  /* Final one */
  {  1*8,  7*8,  1,  7,
    19*8,  7*8, 19,  7,
    NAMED_VALUES_1("Change direction", 0)
  },
  {0}
};

TELEPORTER_DEFINITION level4_teleporters[] = {
  {  1*8,  1*8,  1,  1,
    20*8, 31*8, 20, 31,
    NAMED_VALUES_1("Change direction", 0)
  },
  {  1*8, 30*8,  1, 30,
    22*8, 11*8, 22, 11,
    NAMED_VALUES_1("Change direction", 1)
  },
  { 12*8,  8*8, 12,  8,
    16*8, 25*8, 16, 25,
    NAMED_VALUES_1("Change direction", 0)
  },
  {0}
};

/***
 *       _____ _                  _
 *      / ____| |                | |
 *     | (___ | | _____      ____| | _____      ___ __  ___
 *      \___ \| |/ _ \ \ /\ / / _` |/ _ \ \ /\ / | '_ \/ __|
 *      ____) | | (_) \ V  V | (_| | (_) \ V  V /| | | \__ \
 *     |_____/|_|\___/ \_/\_/ \__,_|\___/ \_/\_/ |_| |_|___/
 *
 *
 */

SLOWDOWN level0_slowdowns[] = {
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
                            "Sprite x", 184, "Sprite y", 176,
                            "Centre x", 188, "Centre y", 180,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(15) },
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
                            "Sprite x",  30, "Sprite y",  64,
                            "Centre x",  34, "Centre y",  68,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(15) },
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
                            "Sprite x", 208, "Sprite y", 104,
                            "Centre x", 210, "Centre y", 108,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(15) },
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
                            "Sprite x",   0, "Sprite y",   0,
                            "Centre x",   0, "Centre y",   0,
                            0,
                            0)}},
};

SLOWDOWN level1_slowdowns[] = {
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
                            "Sprite x", 180, "Sprite y", 128,
                            "Centre x", 184, "Centre y", 132,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(15)  },
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
                            "Sprite x", 240, "Sprite y", 88,
                            "Centre x", 244, "Centre y", 92,
                            slowdown_collected,
                            slowdown_timeup)},  SLOWDOWN_SECS(12)  },
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
                            "Sprite x",   0, "Sprite y",   0,
                            "Centre x",   0, "Centre y",   0,
                            0,
                            0)}},
};

SLOWDOWN level2_slowdowns[] = {
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
                            "Sprite x", 14*8,   "Sprite y", 20*8,
                            "Centre x", 14*8+4, "Centre y", 20*8+4,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(12)  },
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
                            "Sprite x", 1*8,    "Sprite y", 11*8,
                            "Centre x", 1*8+4,  "Centre y", 11*8+4,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(15)  },
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
                            "Sprite x", 30*8,   "Sprite y", 20*8,
                            "Centre x", 30*8+4, "Centre y", 20*8+4,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(15)  },
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
                            "Sprite x", 23*8,   "Sprite y",  9*8,
                            "Centre x", 23*8+4, "Centre y",  9*8+4,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(15)  },
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
                            "Sprite x",   0, "Sprite y",   0,
                            "Centre x",   0, "Centre y",   0,
                            0,
                            0)}},
};

SLOWDOWN level3_slowdowns[] = {
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
			    "Sprite x", 16*8,   "Sprite y",  4*8,
                            "Centre x", 16*8+4, "Centre y",  4*8+4,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(5)  },
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
			    "Sprite x", 16*8,   "Sprite y", 22*8,
                            "Centre x", 16*8+4, "Centre y", 22*8+4,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(7)  },
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
			    "Sprite x",   0, "Sprite y",   0,
                            "Centre x",   0, "Centre y",   0,
                            0,
                            0)}},
};

SLOWDOWN level4_slowdowns[] = {
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
	                   		    "Sprite x", 28*8,   "Sprite y",  8*8,
                            "Centre x", 28*8+4, "Centre y",  8*8+4,
                            slowdown_collected,
                            slowdown_timeup)}, SLOWDOWN_SECS(8)  },
  { {INITIALISE_COLLECTABLE(SLOWDOWN_PILL,
                            "Sprite x",   0, "Sprite y",   0,
                            "Centre x",   0, "Centre y",   0,
                            0,
                            0)}},
};


/***
 *      _____
 *     |  __ \
 *     | |  | | ___   ___  _ __ ___
 *     | |  | |/ _ \ / _ \| '__/ __|
 *     | |__| | (_) | (_) | |  \__ \
 *     |_____/ \___/ \___/|_|  |___/
 *
 *
 */

DOOR level2_doors[] = {
  { {INITIALISE_COLLECTABLE(DOOR_KEY,
                            "Key x cell",       8,  "Key y cell",        4,
                            "Centre x px", (8*8+4), "Centre y px", (4*8+4),
                            door_key_collected,
                            door_open_timeup)},
    INITIALISE_DOOR_LOCATION(17,22),
    NAMED_VALUES_1("Door ink",     INK_MAGENTA),
    NAMED_VALUES_1("Key ink",        INK_WHITE),
    NAMED_VALUES_1("Key paper",      INK_BLACK),
    NAMED_VALUES_1("Door open secs",        10),
    NAMED_VALUES_1("Start open secs",        2),
  },
  { {INITIALISE_COLLECTABLE(DOOR_KEY,
                            "Key x cell",       5,  "Key y cell",       22,
                            "Centre x px", (5*8+4), "Centre y px", (22*8+4),
                            door_key_collected,
                            door_open_timeup)},
    INITIALISE_DOOR_LOCATION(22,22),
    NAMED_VALUES_1("Door ink",        INK_BLUE),
    NAMED_VALUES_1("Key ink",        INK_WHITE),
    NAMED_VALUES_1("Key paper",      INK_BLACK),
    NAMED_VALUES_1("Door open secs",         5),
    NAMED_VALUES_1("Start open secs",        3),
  },
  { {INITIALISE_COLLECTABLE(DOOR_KEY,
                            "Key x cell",       30,  "Key y cell",        4,
                            "Centre x px", (30*8+4), "Centre y px", (4*8+4),
                            door_key_collected,
                            door_open_timeup)},
    INITIALISE_DOOR_LOCATION(27,22),
    NAMED_VALUES_1("Door ink",       INK_GREEN),
    NAMED_VALUES_1("Key ink",        INK_WHITE),
    NAMED_VALUES_1("Key paper",      INK_BLACK),
    NAMED_VALUES_1("Door open secs",         6),
    NAMED_VALUES_1("Start open secs",        4),
  },
  { {INITIALISE_COLLECTABLE(DOOR_KEY,
                            "Sprite x",   0, "Sprite y",   0,
                            "Centre x",   0, "Centre y",   0,
                            0,
                            0)}, 0 },
};

DOOR level3_doors[] = {
  { {INITIALISE_COLLECTABLE(DOOR_KEY,
                            "Key x cell",       30,  "Key y cell",        22,
                            "Centre x px", (30*8+4), "Centre y px", (22*8+4),
                            door_key_collected,
                            door_open_timeup)},
    INITIALISE_DOOR_LOCATION(4,1),
    NAMED_VALUES_1("Door ink",       INK_RED),
    NAMED_VALUES_1("Key ink",        INK_BLACK),
    NAMED_VALUES_1("Key paper",      INK_WHITE),
    NAMED_VALUES_1("Door open secs",        12),
    NAMED_VALUES_1("Start open secs",        3),
  },
  { {INITIALISE_COLLECTABLE(DOOR_KEY,
                            "Sprite x",   0, "Sprite y",   0,
                            "Centre x",   0, "Centre y",   0,
                            0,
                            0)}, 0 },
};

DOOR level4_doors[] = {
  { {INITIALISE_COLLECTABLE(DOOR_KEY,
                            "Key x cell",        6,  "Key y cell",         1,
                            "Centre x px",  (6*8+4), "Centre y px",  (1*8+4),
                            door_key_collected,
                            door_open_timeup)},
    INITIALISE_DOOR_LOCATION(9,22),
    NAMED_VALUES_1("Door ink",       INK_GREEN),
    NAMED_VALUES_1("Key ink",        INK_WHITE),
    NAMED_VALUES_1("Key paper",      INK_BLACK),
    NAMED_VALUES_1("Door open secs",         6),
    NAMED_VALUES_1("Start open secs",        0),
  },
  { {INITIALISE_COLLECTABLE(DOOR_KEY,
                            "Key x cell",       24,  "Key y cell",         4,
                            "Centre x px",  (24*8+4), "Centre y px",  (4*8+4),
                            door_key_collected,
                            door_open_timeup)},
    INITIALISE_DOOR_LOCATION(8,4),
    NAMED_VALUES_1("Door ink",       INK_RED),
    NAMED_VALUES_1("Key ink",        INK_WHITE),
    NAMED_VALUES_1("Key paper",      INK_BLACK),
    NAMED_VALUES_1("Door open secs",        10),
    NAMED_VALUES_1("Start open secs",        0),
  },
  { {INITIALISE_COLLECTABLE(DOOR_KEY,
                            "Key x cell",        7,  "Key y cell",         4,
                            "Centre x px",  (7*8+4), "Centre y px",  (4*8+4),
                            door_key_collected,
                            door_open_timeup)},
    INITIALISE_DOOR_LOCATION(11,12 ),
    NAMED_VALUES_1("Door ink",       INK_YELLOW),
    NAMED_VALUES_1("Key ink",        INK_WHITE),
    NAMED_VALUES_1("Key paper",      INK_BLACK),
    NAMED_VALUES_1("Door open secs",        10),
    NAMED_VALUES_1("Start open secs",        0),
  },
  { {INITIALISE_COLLECTABLE(DOOR_KEY,
                            "Key x cell",        23,  "Key y cell",       12,
                            "Centre x px",  (23*8+4), "Centre y px",  (12*8+4),
                            door_key_collected,
                            door_open_timeup)},
    INITIALISE_DOOR_LOCATION(2,14),
    NAMED_VALUES_1("Door ink",       INK_CYAN),
    NAMED_VALUES_1("Key ink",        INK_WHITE),
    NAMED_VALUES_1("Key paper",      INK_BLACK),
    NAMED_VALUES_1("Door open secs",        6),
    NAMED_VALUES_1("Start open secs",        0),
  },
  { {INITIALISE_COLLECTABLE(DOOR_KEY,
                            "Sprite x",   0, "Sprite y",   0,
                            "Centre x",   0, "Centre y",   0,
                            0,
                            0)}, 0 },
};



/***
 *      _                    _
 *     | |                  | |
 *     | |     _____   _____| |___
 *     | |    / _ \ \ / / _ | / __|
 *     | |___|  __/\ V |  __| \__ \
 *     |______\___| \_/ \___|_|___/
 *
 *
 */

LEVEL_DATA level_data[] = {



/***
 *      ___     _             _                _ 
 *     |_ _|_ _| |_ _ _ ___  | |   _____ _____| |
 *      | || ' |  _| '_/ _ \ | |__/ -_\ V / -_| |
 *     |___|_||_\__|_| \___/ |____\___|\_/\___|_|
 *                                               
 */
  {
    0,
    level_intro_map,
    START_POINT(100,0),
    LEVEL_BORDER(INK_BLACK),
    START_FACING(RIGHT),
    NAMED_VALUES_3("Background", INK_BLACK|PAPER_WHITE,
                   "Solid",      INK_GREEN|PAPER_WHITE,
                   "Jumper",     INK_RED|PAPER_GREEN),
    &level0_tiles[0],
    NULL,
    NULL,
    NULL,
    { NAMED_VALUES_5("Countdown slider X", 255,  /* I don't want bonuses showing on the intro */
                     "Countdown slider Y", 255,  /* level so they actually get drawn off screen */
                     "Scores att",         0,
                     "Bonus X pixel",      255,
                     "Bonus Y pixel",      255) }
  },

  /***
   *      _                _    __
   *     | |   _____ _____| |  /  \
   *     | |__/ -_\ V / -_| | | () |
   *     |____\___|\_/\___|_|  \__/
   *
   */
  {
    1,
    level0_map,
    START_POINT(3,140),
    LEVEL_BORDER(INK_RED),
    START_FACING(RIGHT),
    NAMED_VALUES_3("Background", INK_BLACK|PAPER_WHITE,
                   "Solid",      INK_GREEN|PAPER_WHITE,
                   "Jumper",     INK_RED|PAPER_GREEN),
    &level0_tiles[0],
    NULL,
    &level0_slowdowns[0],
    NULL,
    { NAMED_VALUES_5("Countdown slider X", 19*8,
                     "Countdown slider Y", 18*8,
                     "Scores att",         INK_BLUE|PAPER_WHITE,
                     "Bonus X pixel",      19*8,
                     "Bonus Y pixel",      19*8) }
  },

  /***
   *      _                _   _
   *     | |   _____ _____| | / |
   *     | |__/ -_\ V / -_| | | |
   *     |____\___|\_/\___|_| |_|
   *
   */
  {
    2,
    level1_map,
    START_POINT(3,155),
    LEVEL_BORDER(INK_BLUE),
    START_FACING(RIGHT),
    NAMED_VALUES_3("Background", INK_MAGENTA|PAPER_BLACK,
                   "Solid",      INK_CYAN|PAPER_BLACK,
                   "Jumper",     INK_RED|PAPER_BLACK),
    &level1_tiles[0],
    &level1_teleporters[0],
    &level1_slowdowns[0],
    NULL,
    { NAMED_VALUES_5("Countdown slider X", 14*8,
                     "Countdown slider Y", 19*8,
                     "Scores att",         INK_YELLOW|PAPER_BLACK,
                     "Bonus X pixel",      14*8,
                     "Bonus Y pixel",      20*8) }
  },

  /***
   *      _                _   ___
   *     | |   _____ _____| | |_  )
   *     | |__/ -_\ V / -_| |  / /
   *     |____\___|\_/\___|_| /___|
   *
   */
  {
    3,
    level2_map,
    START_POINT(3,163),
    LEVEL_BORDER(INK_BLACK),
    START_FACING(RIGHT),
    NAMED_VALUES_3("Background", INK_WHITE|PAPER_BLACK,
                   "Solid",      INK_YELLOW|PAPER_BLACK,
                   "Jumper",     INK_RED|PAPER_BLACK),
    &level2_tiles[0],
    &level2_teleporters[0],
    level2_slowdowns,
    &level2_doors[0],
    { NAMED_VALUES_5("Countdown slider X",  0*8,
                     "Countdown slider Y",  0*8,
                     "Scores att",          INK_WHITE|PAPER_BLACK,
                     "Bonus X pixel",       0*8,
                     "Bonus Y pixel",       1*8) }
  },

  /***
   *      _                _   ____
   *     | |   _____ _____| | |__ /
   *     | |__/ -_\ V / -_| |  |_ \
   *     |____\___|\_/\___|_| |___/
   *
   */
  {
    4,
    level3_map,
    START_POINT(11,16),
    LEVEL_BORDER(INK_RED),
    START_FACING(RIGHT),
    NAMED_VALUES_3("Background", INK_BLACK|PAPER_WHITE,
                   "Solid",      INK_RED|PAPER_YELLOW,
                   "Jumper",     INK_RED|PAPER_WHITE),
    level3_tiles,
    level3_teleporters,
    level3_slowdowns,
    level3_doors,
    { NAMED_VALUES_5("Countdown slider X", 21*8,
                     "Countdown slider Y",  0*8,
                     "Scores att",         INK_BLUE|PAPER_WHITE,
                     "Bonus X pixel",      21*8,
                     "Bonus Y pixel",       1*8) }
  },

  /***
   *      _                _   _ _
   *     | |   _____ _____| | | | |
   *     | |__/ -_\ V / -_| | |_  _|
   *     |____\___|\_/\___|_|   |_|
   *
   */
  {
    5,
    level4_map,
    START_POINT(11,0),
    LEVEL_BORDER(INK_BLACK),
    START_FACING(RIGHT),
    NAMED_VALUES_3("Background", INK_WHITE|PAPER_BLACK,
                   "Solid",      INK_MAGENTA|PAPER_BLACK,
                   "Jumper",     INK_RED|PAPER_BLACK),
    level4_tiles,
    level4_teleporters,
    level4_slowdowns,
    level4_doors,
    { NAMED_VALUES_5("Countdown slider X", 20*8,
                     "Countdown slider Y", 21*8,
                     "Scores att",         INK_GREEN|PAPER_BLACK,
                     "Bonus X pixel",      20*8,
                     "Bonus Y pixel",      22*8) }
  },

};


/*
 * Font is tucked away in levels_graphics.asm
 */
extern uint8_t font[];

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
    print_string[1] = TELEPORTER_INK;
    print_string[2] = '\x11'; /* Paper <n> */
    print_string[3] = TELEPORTER_PAPER;

    print_string[4] = '\x16'; /* AT <x>,<y> */

    print_string[7] = TELEPORTER_UDG;
    print_string[8] = '\0';

    while( teleporter->end_1_x || teleporter->end_1_y )
    {
      print_string[5] = teleporter->end_1_y_cell;
      print_string[6] = teleporter->end_1_x_cell;

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

void setup_levels_font( void )
{
  uint8_t          i;

  /*
   * At the moment all levels use the same font, so this is
   * hardcoded to the data pointer
   */
  uint8_t*         font_ptr = font;

  /*
   * The font is hardcoded to contain 96 chars for space (32d) onwards.
   * I don't need them all so there's a saving to be made here if I need to.
   */
  for( i = 0; i < 96; i++ )
  {
     sp1_TileEntry( i+32, font_ptr );
     font_ptr += 8;
  }
}
