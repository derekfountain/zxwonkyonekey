#include <stdint.h>
#include <arch/zx/sp1.h>

#include "runner.h"
#include "game_state.h"

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

/*
 * Runner has access to full screen for now. It'll be faster
 * when I can reduce this window size.
 */
const struct sp1_Rect runner_screen = {0, 0, 32, 24};

/*
 * Table of offsets in the y-position the runner takes as he
 * goes through the jump animation.
 * This data set makes the jump 5 chars wide, with a sharp
 * incline at the start and decline at the end, so like an
 * arc. Arrived at emprically.
 * The negative values are probably a bit expensive on the
 * Z80 but this appears to be the easiest way to do it.
 */
const int8_t jump_y_offsets[] =  { 2,  2,  2,  2,    2,  1,  1,  1,
			           1,  1,  1,  1,    1,  1,  1,  0,
                                   0,  0,  0,  0,    0,  0,  0,  0,
		                   0, -1, -1, -1,   -1, -1, -1, -1,
			          -1, -1, -1, -2,   -2, -2, -2, -2 };

/*
 * For flexibility I want to do something like:
 *
 *  static const uint16_t runner_frame_size = runner_right_f2-runner_right_f1;
 *
 * but the compiler complains that those values aren't constants. So maybe
 * there's a way to export a DEFC'ed value from the assembler to the C?
 * I can't find a way to do that either. So for now use a hard coded constant.
 *
 * Futhermore, this:
 *
 *   static const uint16_t runner_frame_size = 16;
 * 
 * actually compiles to a value held in memory, which is a bit crap.
 * Using a constant of 16 compiles to 4 'add hl,hl' instructions.
 */
#define RUNNER_FRAME_SIZE (uint16_t)16

/*
 * Structure to look after the runner character.
 */
static RUNNER runner;

RUNNER* create_runner( DIRECTION initial_direction )
{
  runner.sprite = sp1_CreateSpr(SP1_DRAW_LOAD1LB, SP1_TYPE_1BYTE, 2, 0, 0);
  sp1_AddColSpr(runner.sprite, SP1_DRAW_LOAD1RB, SP1_TYPE_1BYTE, 0, 0);

  runner.xpos = 0;
  runner.ypos = 0;
  runner.facing      = initial_direction;
  runner.jump_offset = NOT_JUMPING;

  return &runner;
}

void position_runner( uint8_t x, uint8_t* y )
{
  uint8_t* runner_data;
  uint16_t offset_to_frame;

  /*
   * Frame number is calculated from the lowest 8 bits of the x position.
   * This won't be flexible enough if the animation gets more complex
   * but for now it's fast and simple.
   *
   * The calculation is, for example:
   * 
   *  frame_num   = x & 0x0007;
   *  runner_data = runner_right_f1+(RUNNER_FRAME_SIZE*frame_num);
   *
   * The implementation below doesn't use the intermediate variable and
   * generates more succinct code.
   */
  offset_to_frame = RUNNER_FRAME_SIZE * (x & 0x0007);

  if( runner.facing == RIGHT ) {
    runner_data = runner_right_f1+offset_to_frame;
  }
  else {
    runner_data = runner_left_f1+offset_to_frame;
  }

  if( RUNNER_JUMPING ) {
    *y -= jump_y_offsets[runner.jump_offset];
    if( ++runner.jump_offset == sizeof(jump_y_offsets) ) {
      runner.jump_offset = NOT_JUMPING;
    }    
  }

  sp1_MoveSprPix(runner.sprite, &runner_screen, runner_data, x, *y);
}

void toggle_runner_direction(void)
{
  /*
   * As long as there are only two directions enumerated as 0 and 1,
   * this 'not' is OK. :)
   */
  runner.facing = !runner.facing;

  /*
   * If the runner is mid-jump, see if he's more than halfway
   * through. Adjust the position of the jump position
   * accordingly to allow the jump to be completed after he's
   * turned around in midair. This was determined empirically;
   * the resultant animation and positioning feel right.
   */
  if( RUNNER_JUMPING )
    if( runner.jump_offset < sizeof(jump_y_offsets)/2 )
      runner.jump_offset = sizeof(jump_y_offsets) - runner.jump_offset;
}

void start_runner_jumping(void)
{
  runner.jump_offset = 0;
}

GAME_ACTION move_sideways( void* data )
{
  GAME_STATE* game_state = (GAME_STATE*)data;

  if( game_state->runner->facing == RIGHT ) {
    return MOVE_RIGHT;
  }
  else {
    return MOVE_LEFT;
  }
}
