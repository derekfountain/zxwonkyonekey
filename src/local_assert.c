/*
 * Wonky One Key, a ZX Spectrum game featuring a single control key
 * Copyright (C) 2018 Derek Fountain
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

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
