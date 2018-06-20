#include <stdint.h>
#include <arch/zx/sp1.h>

#include "runner.h"

extern uint8_t runner_right_f1[];
extern uint8_t runner_right_f2[];
extern uint8_t runner_right_f3[];
extern uint8_t runner_right_f4[];
extern uint8_t runner_right_f5[];
extern uint8_t runner_right_f6[];
extern uint8_t runner_right_f7[];
extern uint8_t runner_right_f8[];

extern uint8_t runner_left_f1[];
extern uint8_t runner_left_f2[];
extern uint8_t runner_left_f3[];
extern uint8_t runner_left_f4[];
extern uint8_t runner_left_f5[];
extern uint8_t runner_left_f6[];
extern uint8_t runner_left_f7[];
extern uint8_t runner_left_f8[];

typedef struct _runner_state
{
  struct sp1_ss*   sprite;

  RUNNER_DIRECTION direction;
  uint8_t          frame_num;
}
RUNNER_STATE;

struct sp1_Rect runner_screen = {0, 0, 32, 24};
uint16_t runner_frame_size;

RUNNER_STATE runner;

uint8_t x_pos = 0;
uint8_t y_pos = 80;

void create_runner_sprite( RUNNER_DIRECTION initial_direction )
{
  runner_frame_size = (runner_right_f2-runner_right_f1);

  runner.sprite = sp1_CreateSpr(SP1_DRAW_LOAD1LB, SP1_TYPE_1BYTE, 2, 0, 0);
  sp1_AddColSpr(runner.sprite, SP1_DRAW_LOAD1RB, SP1_TYPE_1BYTE, 0, 0);

  runner.direction = initial_direction;
  runner.frame_num = 0;
}

void position_runner( uint8_t* x, uint8_t* y )
{
  uint8_t* runner_data;

/* This didn't work properly. Animation looked weird. */
  runner.frame_num = (*x)&0x0007;

  if( runner.direction == RUNNER_RIGHT ) {
    runner_data = runner_right_f1+(runner_frame_size*runner.frame_num);
    (*x)++;
  }
  else {
    runner_data = runner_left_f1+(runner_frame_size*runner.frame_num);
    (*x)--;
  }

  sp1_MoveSprPix(runner.sprite, &runner_screen, runner_data, *x, *y);

//  if( ++runner.frame_num == 8 )
//    runner.frame_num = 0;
}

void change_runner_direction(void)
{
  runner.direction = !runner.direction;
}
