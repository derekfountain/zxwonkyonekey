#ifndef __GAME_STATE_H
#define __GAME_STATE_H

#include <stdint.h>

typedef struct _game_state
{
  uint8_t key_pressed;
  uint8_t key_processed;

  uint8_t player_xpos;
  uint8_t player_ypos;
}
GAME_STATE;

#endif
