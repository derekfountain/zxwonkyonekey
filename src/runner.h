#ifndef __RUNNER_H
#define __RUNNER_H

/* Directions. Up and down will probably be added */
typedef enum _runner_direction
{
  RUNNER_RIGHT=0,
  RUNNER_LEFT =1,
}
RUNNER_DIRECTION;

void create_runner_sprite( RUNNER_DIRECTION initial_direction );
void position_runner( uint8_t* x, uint8_t* y );
void toggle_runner_direction( void );
void start_runner_jumping( void );

#endif
