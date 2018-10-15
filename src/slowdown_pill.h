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

#ifndef __SLOWDOWN_PILL_H
#define __SLOWDOWN_PILL_H

typedef enum _slowdown_status
{
  SLOWDOWN_INACTIVE,
  SLOWDOWN_ACTIVE,
} SLOWDOWN_STATUS;

typedef enum _pill_availability
{
  PILL_AVAILABLE,
  PILL_NOT_AVAILABLE,
} PILL_AVAILABILITY;

/*
 * Slowdown point, stored as an x,y pixel location. This structure also
 * contains the sprite and animation bits.
 */
typedef struct _slowdown_definition
{
  /* Keep these first because they're statically initialised in the level data */

  /* x,y pixel position to place the sprite */
  uint8_t            x;
  uint8_t            y;

  /*
   * The x,y position is the top left corner of the sprite. For collision purposes
   * I want to check on the centre of sprite. This location is x,y plus 3 pixels
   * in both directions, but since it's used every frame I store the values rather
   * than do those additions every time.
   */
  uint8_t            centre_x;
  uint8_t            centre_y;

  /*
   * Is pill available? This starts as yes, and is set to no when it gets eaten.
   * The graphic is then removed from screen. It's reset yes when the timer
   * expires and it reappears.
   */
  PILL_AVAILABILITY  available;
  uint8_t            duration_secs;

  /* These are not statically initialised */
  struct sp1_ss* sprite;
  uint8_t        frame;
  uint8_t        expanding;

  uint16_t       complete_timer; 

} SLOWDOWN_DEFINITION;


void create_slowdown_pill( SLOWDOWN_DEFINITION* slowdown );
void destroy_slowdown_pill( SLOWDOWN_DEFINITION* slowdown );
void animate_slowdown_pill( SLOWDOWN_DEFINITION* slowdown );

#endif
