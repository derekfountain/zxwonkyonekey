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


;; THIS IS NO LONGER USED IN THE CODE. MIGHT BE HANDY FOR DEBUGGING THOUGH.


;; This exposes the Z88DK internal unsigned int to ascii routine, which is
;; more efficient than the one supplied in the libraries. It prints the
;; value as a 5 char string, the scf meaning "with leading zeroes".
;;
;; See this thread at Spectrum Computing for a discussion on this:
;;
;; https://spectrumcomputing.co.uk/forums/viewtopic.php?f=6&t=1031

; char *small_utoa(unsigned int num, char *buf)

SECTION code_stdlib

PUBLIC _small_utoa

EXTERN l_small_utoa

_small_utoa:

   pop af
   pop hl
   pop de
   pop bc

   push bc
   push de
   push hl
   push af

   scf
   call l_small_utoa
   ret
