#ifndef __GAME_STATE_H
#define __GAME_STATE_H

#include <stdint.h>

#include "runner.h"

/*
 * This structure defines the state of the game.
 *
 * There's probably level information, number of lives left,
 * and goodness knows what else to be added here.
 */
typedef struct _game_state
{
  uint8_t key_pressed;
  uint8_t key_processed;

  RUNNER* runner;
} GAME_STATE;

#endif
