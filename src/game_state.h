#ifndef __GAME_STATE_H
#define __GAME_STATE_H

#include <stdint.h>

#include "runner.h"

typedef struct _game_state
{
  uint8_t key_pressed;
  uint8_t key_processed;

  RUNNER* runner;
}
GAME_STATE;

#endif
