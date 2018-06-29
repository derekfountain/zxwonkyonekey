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
#include <stdio.h>
#include "utils.h"
#include "action.h"
#include "game_state.h"

/*
TODO

Replace PAPER_WHITE with a value from the level data (empty sky).
Also green (walls) and red (killer)
Level data needs to be in the game state.

 */

/***
 *      _____  _               _   _                _____ _                            
 *     |  __ \(_)             | | (_)              / ____| |                           
 *     | |  | |_ _ __ ___  ___| |_ _  ___  _ __   | |    | |__   __ _ _ __   __ _  ___ 
 *     | |  | | | '__/ _ \/ __| __| |/ _ \| '_ \  | |    | '_ \ / _` | '_ \ / _` |/ _ \
 *     | |__| | | | |  __| (__| |_| | (_) | | | | | |____| | | | (_| | | | | (_| |  __/
 *     |_____/|_|_|  \___|\___|\__|_|\___/|_| |_|  \_____|_| |_|\__,_|_| |_|\__, |\___|
 *                                                                           __/ |     
 *                                                                          |___/      
 * Test for direction change.
 *
 * Runner changes direction if he hits a wall or the user
 * presses the control key.
 *
 * Returns TOGGLE_DIRECTION if the runner's direction is to
 * be reversed.
 */
GAME_ACTION test_for_direction_change( void* data )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address;

  /* If he's not on a character cell boundary he can't be up against a wall */
  if( MODULO8(game_state->runner->xpos) == 0 ) {

    /* Pick up the attributes of the char cell he's facing and about to move into */
    if( game_state->runner->facing == RIGHT )
      attr_address = zx_pxy2aaddr( game_state->runner->xpos+8, game_state->runner->ypos );
    else
      attr_address = zx_pxy2aaddr( game_state->runner->xpos-8, game_state->runner->ypos );
  
    if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE )
      return TOGGLE_DIRECTION;
  }

  if( game_state->key_pressed && ! game_state->key_processed ) {
    game_state->key_processed = 1;
    return TOGGLE_DIRECTION;
  }

  return NO_ACTION;
}



/***
 *           _                      ___  
 *          | |                    |__ \ 
 *          | |_   _ _ __ ___  _ __   ) |
 *      _   | | | | | '_ ` _ \| '_ \ / / 
 *     | |__| | |_| | | | | | | |_) |_|  
 *      \____/ \__,_|_| |_| |_| .__/(_)  
 *                            | |        
 *                            |_|        
 * Test for making runner jump.
 *
 * Runner jumps if:
 *
 *  he's not already jumping; and
 *  part of him is on a trampoline block; and
 *  player hits the control key
 */
GAME_ACTION test_for_start_jump( void* data )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address;

  /* Are we already jumping? If so, no action */
  if( RUNNER_JUMPING(game_state->runner->jump_offset) )
    return NO_ACTION;

  /* Is the cell below him a trampoline block? If not, no action */
  attr_address = zx_pxy2aaddr( game_state->runner->xpos, game_state->runner->ypos+8  );

  if( (*attr_address & ATTR_MASK_PAPER) != PAPER_RED ) {

    /* If the cell below isn't a trampoline, and he's aligned right on top of it, no jump */
    if( MODULO8(game_state->runner->xpos) == 0 )
      return NO_ACTION;
      
    /* He's not aligned, so check the cell he's partially over too */
    attr_address = zx_pxy2aaddr( game_state->runner->xpos+8, game_state->runner->ypos+8  );

    /* If that's not a a trampoline, no jump */
    if( (*attr_address & ATTR_MASK_PAPER) != PAPER_RED )
      return NO_ACTION;
  }

  /* If we get here, at least part of him is on a trampoline */
  if( game_state->key_pressed && ! game_state->key_processed ) {
    game_state->key_processed = 1;
    return JUMP;
  }

  return NO_ACTION;
}




/***
 *      ______    _ _ ___  
 *     |  ____|  | | |__ \ 
 *     | |__ __ _| | |  ) |
 *     |  __/ _` | | | / / 
 *     | | | (_| | | ||_|  
 *     |_|  \__,_|_|_|(_)  
 *                         
 * Test for making runner fall:
 *
 * Runner falls if:
 *
 *  he's not in the middle of a jump; and
 *  the attribute cell underneath him is clear; and
 *  he's aligned on top of that cell
 */
GAME_ACTION test_for_falling( void* data )
{
  GAME_STATE* game_state = (GAME_STATE*)data;
  uint8_t*    attr_address;

  /* Are we in the middle of a jump? If so, no action */
  if( RUNNER_JUMPING(game_state->runner->jump_offset) )
    return NO_ACTION;

  /* Are we on a coloured block? If not, no action */
  attr_address = zx_pxy2aaddr( game_state->runner->xpos, game_state->runner->ypos+8  );
  
  if( (*attr_address & ATTR_MASK_PAPER) != PAPER_WHITE )
    return NO_ACTION;

  /* So player is on a clear block. If aligned to the cell, fall through */
  if( MODULO8(game_state->runner->xpos) == 0 )
    return MOVE_DOWN;

  return NO_ACTION;
}
