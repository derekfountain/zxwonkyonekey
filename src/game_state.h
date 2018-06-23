#ifndef __GAME_STATE_H
#define __GAME_STATE_H

#include <stdint.h>

#include "runner.h"

typedef struct _game_state
{
  uint8_t key_pressed;
  uint8_t key_processed;

  RUNNER* runner;

  uint8_t player_xpos;
  uint8_t player_ypos;
}
GAME_STATE;

#endif
