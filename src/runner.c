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
}
RUNNER_STATE;
RUNNER_STATE runner;

/*
 * Runner has access to full screen for now. It'll be faster
 * when I can reduce this window size.
 */
const struct sp1_Rect runner_screen = {0, 0, 32, 24};

/*
 * For flexibility I want to do something like:
 *
 *  static const uint16_t runner_frame_size = runner_right_f2-runner_right_f1;
 *
 * but the compiler complains that those values aren't constants. So maybe
 * there's a way to export a DEFC'ed value from the assembler to the C?
 * I can't find a way to do that either. So for now use a hard coded constant.
 */

static const uint16_t runner_frame_size = 16;


void create_runner_sprite( RUNNER_DIRECTION initial_direction )
{
  runner.sprite = sp1_CreateSpr(SP1_DRAW_LOAD1LB, SP1_TYPE_1BYTE, 2, 0, 0);
  sp1_AddColSpr(runner.sprite, SP1_DRAW_LOAD1RB, SP1_TYPE_1BYTE, 0, 0);

  runner.direction = initial_direction;
}

void position_runner( uint8_t* x, uint8_t* y )
{
  uint8_t* runner_data;
  uint16_t offset_to_frame;

  /*
   * Frame number is calculated from the lowest 8 bits of the x position.
   * This won't be flexible enough if the animation gets more complex
   * but for now it's fast and simple.
   *
   * The calculation is:
   * 
   *  frame_num   = (*x) & 0x0007;
   *  runner_data = runner_right_f1+(runner_frame_size*frame_num);
   *
   * The implementation below doesn't use the intermediate variable and
   * generates more succinct code.
   */
  offset_to_frame = runner_frame_size * ((*x) & 0x0007);

  if( runner.direction == RUNNER_RIGHT ) {
    runner_data = runner_right_f1+offset_to_frame;
    (*x)++;
  }
  else {
    runner_data = runner_left_f1+offset_to_frame;
    (*x)--;
  }

  sp1_MoveSprPix(runner.sprite, &runner_screen, runner_data, *x, *y);
}

void toggle_runner_direction(void)
{
  runner.direction = !runner.direction;
}
