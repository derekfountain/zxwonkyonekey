#ifdef NDEBUG

/*
 * The compiler complains about a empty source file.
 * I had trouble finding something which didn't equate
 * to nothing, but produced no code in the object.
 * This seems to do the trick. :)
 */
#include <stdint.h>

#else

/*
 * The z88dk assert.h macro results in an fputs(),
 * which requires stdio, which requires one of the
 * fatter CRTs. This version is much lighter and
 * still does what I want.
 *
 * grep the map file for the local_assert_bp
 * symbol and put a breakpoint on it. With a recent
 * fuse, which supports the python debugger interface,
 * something like this:
 *
 * PYTHONPATH=. ./fuse --debugger-command "break $(grep -P '^_local_assert_bp' zxrunner.map | perl -ne '/(\$\w\w\w\w)/ && print "$1"')"
 */

void local_assert_bp(void)
{
  while(1);
}


#endif
