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
#include <string.h>
#include <arch/zx.h>
#include <intrinsic.h>
#include <stdlib.h>

#include "int.h"
#include "bonus.h"

/*
 * Off-screen buffer to put the display into. This is blitted into the screen, replacing
 * whatever the user's program happens to have put there. A "merge" would be friendlier. :)
 * Defined in ASM so it can be stored in lower memory.
 */
extern uint8_t off_screen_buffer[32*8];

/* These are in ASM, so they can be forced into lower memory */
extern uint8_t arch_pattern_l[4][2];
extern uint8_t arch_pattern_r[4][2];
extern int8_t  cascade_pattern[6][2];

extern uint8_t winner_string[];
extern uint8_t winner_bonus_char[];
extern uint8_t failure_string[];

/*
 * Font is tucked away in levels_graphics.asm
 */
extern uint8_t font[];

void banner( uint8_t* string )
{
  uint8_t* buffer_address;
  uint8_t  i;

  /* Address in ROM of the character being scrolled into view */
  uint8_t* rom_address;

  /* Bit, left to right, of the character to scroll into view next. Goes 128, 64, 32...1 */
  uint8_t  bit;

  uint8_t* current_char_ptr;

  zx_cls(PAPER_WHITE);

  /*
   * Initialise the ticker and its buffer
   */
  memset( off_screen_buffer, PAPER_WHITE+INK_WHITE, sizeof(off_screen_buffer) );

  current_char_ptr = string;
  rom_address      = ((*current_char_ptr-0x20)*8)+(uint8_t*)font;
  bit              = 128;

  while( 1 ){
    /*
     * Scroll off-screen display buffer data leftwards one byte.
     * This is just a memory move downwards by one. 
     */
    memcpy((uint8_t*)off_screen_buffer, (uint8_t*)off_screen_buffer+1,
           sizeof(off_screen_buffer)-1);

    /*
     * For each of the 8 lines (top to bottom) of the character we're displaying, pick out
     * the current bit (left to right). If it's a 1, set the rightmost attribute cell to
     * colour, otherwise set the attribute cell white. This is done in the off-screen buffer.
     */
    buffer_address = (uint8_t*)&off_screen_buffer+0x1f;
    for( i=0; i<8; i++ )
    {
      uint8_t attribute_value;

      attribute_value = ( *rom_address & bit ) ? PAPER_MAGENTA : PAPER_WHITE;

      *buffer_address = attribute_value;
      buffer_address += 0x20;

      rom_address++;
    }

    /*
     * If that was the rightmost bit of the character, that character's done with. Move to the
     * next character in the display string and start again at its left side (bit 128).
     * Otherwise keep with the same character and get ready for the next bit.
     */
    if( bit == 1 )
    {
      current_char_ptr++;
      if( *current_char_ptr == '\0' )
        break;

      rom_address = ((*current_char_ptr-0x20)*8)+(uint8_t*)font;

      bit = 128;
    }
    else
      {
        bit = bit/2;

        /* Still on the same character, so move back to the start of its data in ROM */
        rom_address -= 8;
      }

    /* Copy the off-screen buffer into the display */
    memcpy( (uint8_t*)0x5800+(5*32), off_screen_buffer, sizeof(off_screen_buffer) );

    intrinsic_halt();
  }

  return;
}

void winner_banner(void)
{
  winner_bonus_char[0] = query_bonuses_left() + 0x30;
  banner( winner_string );
}

void loser_banner(void)
{
  banner( failure_string );
}

/*
 * Infinite fireworks display to end the game. It's not exactly brilliant
 * but it rounds things off.
 * A firework animation is pre-calculated. There's a stem which goes straight
 * up, then an arch either left or right, then a cascade of colour as it
 * "explodes". Yeah, well, kinda. :)
 * The firework is plotted in the attribute space, arranged so it won't spill
 * outside of the screen limits. The patterns are defined in the tables of
 * offsets above. When the graphic has been pre-calculated in memory it's
 * drawn on screen by setting the attributes of the colour squares in the
 * display. Each firework cell is drawn, with an erase following behind. The
 * effect is supposed to look like a firework rising, exploding, then fading.
 * It sort of works. :)
 */
#define CASCADE_WIDTH  8
#define CASCADE_HEIGHT 5
#define MAX_PAIRS_PER_FIREWORK ( (24-CASCADE_HEIGHT) + 4 + 6 )

/*
 * A firework is plotted as a sequence of x,y attribute location pairs.
 * The height of the stem is random, so this array needs to be big
 * enough to handle the tallest firework. Array is in lower memory.
 */
extern uint8_t pre_calc_path[MAX_PAIRS_PER_FIREWORK][2];

void winner_fireworks(void)
{
  zx_cls(PAPER_BLACK);
  zx_border(INK_BLACK);

  srand( ticker );
  while(1)
  {


    uint8_t height    = (rand()%(24-CASCADE_HEIGHT))+CASCADE_HEIGHT;
    uint8_t xpos      = (rand()%(32-(CASCADE_WIDTH*2)))+CASCADE_WIDTH;
    uint8_t direction = (rand()&0x01);

    uint8_t array_index;
    int8_t* arch_pattern;
    uint8_t arch_index;
    uint8_t cascade_index;

    uint8_t y = 24;

    /* Build the stem */
    array_index = 0;
    {
      uint8_t i = 0;
      for( i=0; i<height; i++ )
      {
        pre_calc_path[array_index][0] = xpos;
        pre_calc_path[array_index][1] = y;
        array_index++;
        y--;
      }
    }    

    /* Add the arch */
    if( direction == 0 )
      arch_pattern = (int8_t*)&arch_pattern_l[0];
    else
      arch_pattern = (int8_t*)&arch_pattern_r[0];

    for( arch_index=0; arch_index<4; arch_index++ )
    {
      pre_calc_path[array_index][0] = xpos+(*(arch_pattern+(2*arch_index)));
      pre_calc_path[array_index][1] = y   +(*(arch_pattern+(2*arch_index)+1));
      array_index++;
    }

    /* Add cascade */
    y++;
    if( direction == 0 )
      xpos -= 4;
    else
      xpos += 4;

    for( cascade_index=0; cascade_index<6; cascade_index++ )
    {
      pre_calc_path[array_index][0] = xpos+cascade_pattern[cascade_index][0];
      pre_calc_path[array_index][1] = y   +cascade_pattern[cascade_index][1];
      array_index++;
    }

    {
      /* Animate the firework */
#define DONT_SHOW 255
#define TRAIL     8

      uint8_t display_index = 0;
      uint8_t remove_index  = DONT_SHOW;
      uint8_t colour = (uint8_t)(((rand()%7)+1)<<3);

      while( 1 )
      {
        if( display_index != DONT_SHOW )
        {
          if( display_index < array_index )
          {
            uint8_t* addr = zx_cxy2aaddr( pre_calc_path[display_index][0],
                                          pre_calc_path[display_index][1] );
            if( display_index < height )
              *addr = PAPER_WHITE|BRIGHT;
            else
              *addr = (uint8_t)(((rand()%7)+1)<<3)|BRIGHT;

            display_index++;
          }
          else
          {
            display_index = DONT_SHOW;
          }
        }

        if( display_index == TRAIL )
          remove_index = 0;

        if( remove_index != DONT_SHOW )
        {
          if( remove_index < array_index )
          {
            uint8_t* addr = zx_cxy2aaddr( pre_calc_path[remove_index][0],
                                          pre_calc_path[remove_index][1] );
            *addr = PAPER_BLACK;

            remove_index++;
          }
          else
          {
            break;
          }
        }

        intrinsic_halt();
      }
    }

  }
}
