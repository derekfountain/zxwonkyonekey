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
#include <malloc.h>
#include <string.h>

#include "hotspot.h"
#include "teleporter.h"

void create_hotspot_list( GAME_STATE* game_state )
{
  HOTSPOT** ptr = &(game_state->hotspot_list);

  if( game_state->current_level->teleporters )
  {
    TELEPORTER_DEFINITION* teleporter = game_state->current_level->teleporters;

    /* Add 2 hotspots to the list for each teleporter */
    while( teleporter->end_1_x || teleporter->end_1_y )
    {
      HOTSPOT* end1 = malloc( sizeof(HOTSPOT) );
      HOTSPOT* end2 = malloc( sizeof(HOTSPOT) );

      *ptr = end1;

      end1->x = teleporter->end_1_x;
      end1->y = teleporter->end_1_y;

      end2->x = teleporter->end_2_x;
      end2->y = teleporter->end_2_y;

      end1->next = end2;
      end2->next = NULL;

      ptr = &(end2->next);

      teleporter++;
    }
  }

  if( game_state->current_level->doors )
  {
    DOOR* door = game_state->current_level->doors;

    /* Loop over doors */
    while( IS_VALID_DOOR(door) )
    {
      HOTSPOT* key_location  = malloc( sizeof(HOTSPOT) );
      HOTSPOT* door_location = malloc( sizeof(HOTSPOT) );

      *ptr = key_location;

      key_location->x = door->collectable.centre_x-4;
      key_location->y = door->collectable.centre_y-4;

      door_location->x = door->door_stays_open_x;
      door_location->y = door->door_stays_open_y;

      key_location->next = door_location;
      door_location->next = NULL;

      ptr = &(door_location->next);

      door++;
    }
  }

  if( game_state->current_level->slowdowns )
  {
    SLOWDOWN* slowdown = game_state->current_level->slowdowns;

    /* Loop over slowdowns */
    while( IS_VALID_SLOWDOWN(slowdown) )
    {
      HOTSPOT* pill_location = malloc( sizeof(HOTSPOT) );

      *ptr = pill_location;

      pill_location->x = slowdown->collectable.centre_x-4;
      pill_location->y = slowdown->collectable.centre_y-4;

      pill_location->next = NULL;

      ptr = &(pill_location->next);

      slowdown++;
    }
  }

}

void teardown_hotspot_list( GAME_STATE* game_state )
{
  HOTSPOT* ptr = game_state->hotspot_list;

  while( ptr )
  {
    HOTSPOT* next = ptr->next;
    free( ptr );

    ptr = next;
  }

  game_state->hotspot_list = NULL;
}

uint8_t is_hotspot( HOTSPOT* hotspot_list, uint8_t x, uint8_t y )
{
  HOTSPOT* ptr = hotspot_list;

  while( ptr )
  {
    if( ptr->y == y && ptr->x == x )
      return 1;

    ptr = ptr->next;
  }

  return 0;
}
