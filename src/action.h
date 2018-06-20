#ifndef __ACTION_H
#define __ACTION_H

typedef enum _game_action
{
  NO_ACTION,
  MOVE_RIGHT,
  MOVE_LEFT,
  MOVE_RIGHT_UP,
  MOVE_RIGHT_DOWN,
  MOVE_LEFT_UP,
  MOVE_LEFT_DOWN,
  MOVE_DOWN,
  TOGGLE_DIRECTION,
  JUMP,
  DIE,
}
GAME_ACTION;

typedef struct _loop_action
{
  GAME_ACTION (*test_action)(void* data);
  void*         data;
}
LOOP_ACTION;

#endif
