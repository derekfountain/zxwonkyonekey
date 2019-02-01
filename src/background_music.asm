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

SECTION code_user

; This goes into the code_user section which is placed by the linker in higher
; memory, above the main compiled C. See this post: https://www.z88dk.org/forum/viewtopic.php?pid=13929
; I originally had this in a section of its own which ended up in lower, contended
; memory. It sounded distinctly odd, so the contended memory thing isn't a myth. :)

PUBLIC _play_note_raw

; hl points to the note to play, passed in from the C. The "note" is actually a delay loop counter which produces
; the correct pitch. That value goes in DE; actually it goes in E because it's 8 bit.
;
; The speaker bit is driven and it goes click. It loops, counting down E, each time driving the speaker with
; an OUT A, but since A isn't changing there's no click. Although this apparently does nothing, the loop is run
; through each time to ensure the CPU executes the exact same instructions, thus keeping the timing consistent.
; When E gets to zero the speaker's bit is toggled; hence, next time round the loop there's a click. Bottom line:
; this implements a click-pause-click-pause loop the speed of which is controlled by the value in DE.
; This loop produces a note of hard coded duration. The duration is controlled by BC. It loops B from 255 to 0,
; C times. C is set to 3, so that's 768 iterations.
;
; 768 iterations of this takes ~31,000 T-states which on the 3.5Mhz Z80 means each note is about 8.8ms. Each game
; cycle in the 50fps game is 20ms, and one note is played every 4 cycles. So every 4th cycle only has about 11ms
; to complete because it spends the rest of the time in this loop. Since the game is simple and 20ms is actually
; ample for each game cycle this is fine. It it weren't I'd have to shift some of the load out of the game cycle
; which plays the note into the other 3 which don't.

EXTERN _GLOBAL_ZX_PORT_FE

_play_note_raw:
    pop     af
    pop     hl
    pop     de
    pop     bc

    push    bc
    push    de
    push    hl
    push    af

	ld      a,(_GLOBAL_ZX_PORT_FE)     ;Pick up current port 0xfe value from z88dk global
    ld      d,0
 	ld      e,(hl) 	                   ;Initialise the pitch delay counter in E
 	ld      bc,3 	                   ;Initialise the duration delay counters in B (0) and C (3)

speaker_loop:
 	out     (254),a 	               ;T=11 Drive the speaker
 	dec     e                          ;T=4
 	jr      nz,pitch_loop_not_done     ;T=12 if met, T=7 if not
 	ld      e,(hl)                     ;T=7  Reset pitch counter and toggle the speaker driver
	xor     0x18                       ;T=7

pitch_loop_not_done:                   ;Countdown B from 256 (i.e. 0) to 0
 	djnz    speaker_loop               ;T=13 if not zero (i.e. it jumps), otherwise T=8

 	dec     c                          ;T=4  BC make the note duration counter
 	jr      nz,speaker_loop            ;T=12 if met, T=7 if not

	ld      (_GLOBAL_ZX_PORT_FE),a     ;Update z88dk's understanding of port 0xfe status

	ret
