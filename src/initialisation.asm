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

;; The main program starts at 32768, which is the start of uncontended
;; memory. That's where Z88DK is configured to put its CRT and main()
;; code, which is correct because that's the fastest memory.
;;
;; I want to use the memory below that (i.e. above the screen and
;; BASIC area but below 0x8000) for static data storage, but appmake
;; generates a BASIC loader which starts at the lowest point in memory.
;; There isn't a way to tell it to place data in low memory, but to start
;; the program from an address in higher memory without writing a custom
;; BASIC loader. Which isn't a great hardship, but this works too. :)

SECTION INIT_CODE

ORG 24950

;; Jump straight to the start of the compiled code

		jp 32768
