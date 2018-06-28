#ifndef __GAMELOOP_H
#define __GAMELOOP_H

#include "game_state.h"

/*
 * This function is the main game loop. It exits when the player dies
 * or completes the level.
 */
void  gameloop( GAME_STATE* game_state );

#endif
