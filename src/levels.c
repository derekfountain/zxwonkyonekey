#include <arch/zx.h>
#include <arch/zx/sp1.h>

#include "levels.h"

void level1(void)
{
  uint8_t i;

  /*
   * I currently have no idea how I'm going to do this.
   */

  sp1_PrintAt(12,0,PAPER_GREEN|INK_BLACK,' ');
  sp1_PrintAt(12,31,PAPER_GREEN|INK_BLACK,' ');

  for(i=0;i<32;i++)
    sp1_PrintAt(13,i,PAPER_GREEN|INK_BLACK,' ');

  sp1_PrintAt(13,15,PAPER_RED|INK_BLACK,' ');

  sp1_PrintAt(13,19,PAPER_WHITE|INK_BLACK,' ');
}
