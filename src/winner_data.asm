;; Wonky One Key, a ZX Spectrum game featuring a single control key
;; Copyright (C) 2018 Derek Fountain
;;
;; This program is free software; you can redistribute it and/or
;; modify it under the terms of the GNU General Public License
;; as published by the Free Software Foundation; either version 2
;; of the License, or (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

SECTION LEVEL_DATA

; These offsets define the patterns in the fireworks at then end of the game.
; I put them here because I'm out of memory above 0x8000 and these can go
; in contended memory without any problems.

PUBLIC _arch_pattern_l
._arch_pattern_l
        defb    -1,  0
        defb    -2, -1
        defb    -3, -1
        defb    -4,  0

PUBLIC _arch_pattern_r
._arch_pattern_r
        defb    +1,  0
        defb    +2, -1
        defb    +3, -1
        defb    +4,  0

PUBLIC _cascade_pattern
._cascade_pattern
        defb    -2, +1
        defb     0, +1
        defb    +2, +1
        defb    -4, +3
        defb     0, +3
        defb    +4, +3

; This buffer is used for the game over message scroller. Again, it's here to
; save a bit of higher memory.

PUBLIC _off_screen_buffer
._off_screen_buffer
        defs    32*8

; Buffer to hold a firework animation:
; uint8_t pre_calc_path[MAX_PAIRS_PER_FIREWORK][2];

PUBLIC _pre_calc_path
._pre_calc_path
        defs    29*2

PUBLIC _ticker_string
._ticker_string
        defm    "You are a winner!  Cue the fireworks!     ", 0
