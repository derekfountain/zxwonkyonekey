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

/*
 * Off-screen buffer to put the display into. This is blitted into the screen, replacing
 * whatever the user's program happens to have put there. A "merge" would be friendlier. :)
 */
static uint8_t  off_screen_buffer[32*8];

void winner_banner(void)
{
  uint8_t* buffer_address;
  uint8_t  i;

  /* Address in ROM of the character being scrolled into view */
  uint8_t* rom_address;

  /* Bit, left to right, of the character to scroll into view next. Goes 128, 64, 32...1 */
  uint8_t  bit;

  uint8_t* current_char_ptr;
  uint8_t  ticker_string[] = "You are a winner!    ";

  zx_cls(PAPER_WHITE);

  /*
   * Initialise the ticker and its buffer
   */
  memset( off_screen_buffer, PAPER_WHITE+INK_WHITE, sizeof(off_screen_buffer) );

  current_char_ptr = ticker_string;
  rom_address      = ((*current_char_ptr-0x20)*8)+(uint8_t*)0x3D00;
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

      rom_address = ((*current_char_ptr-0x20)*8)+(uint8_t*)0x3D00;

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

void winner_fireworks(void)
{
  while(1);
}
