#ifndef __RUNNER_H
#define __RUNNER_H

#define NOT_JUMPING (uint8_t)(0xFF)

/* Directions. Up and down will probably be added */
typedef enum _runner_direction
{
  RUNNER_RIGHT=0,
  RUNNER_LEFT =1,
}
RUNNER_DIRECTION;

typedef struct _runner_state
{
  struct sp1_ss*   sprite;

  RUNNER_DIRECTION direction;
  uint8_t          jump_offset;
}
RUNNER_STATE;

RUNNER_STATE* create_runner_sprite( RUNNER_DIRECTION initial_direction );
void position_runner( uint8_t* x, uint8_t* y );
void toggle_runner_direction( void );
void start_runner_jumping( void );

#endif
