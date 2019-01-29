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
ORG 25000

;;  In the SP1 print string routine, embedded paper colours are
;;  set using 3 bit colour values, i.e. the same as INK.
;;  Don't use the PAPER_* macro values here.

        


;;    _____       _               _                    _ 
;;   |_   _|     | |             | |                  | |
;;     | |  _ __ | |_ _ __ ___   | |     _____   _____| |
;;     | | | '_ \| __| '__/ _ \  | |    / _ \ \ / / _ | |
;;    _| |_| | | | |_| | | (_) | | |___|  __/\ V |  __| |
;;   |_____|_| |_|\__|_|  \___/  |______\___| \_/ \___|_|
;;                                                       
;;                                                       

PUBLIC _level_intro_map
._level_intro_map

INCLUDE "level_intro_map.inc.asm"
        defb 0x00
	
PUBLIC _level_intro_map_end
._level_intro_map_end

        
;;    _                    _    ___  
;;   | |                  | |  / _ \ 
;;   | |     _____   _____| | | | | |
;;   | |    / _ \ \ / / _ | | | | | |
;;   | |___|  __/\ V |  __| | | |_| |
;;   |______\___| \_/ \___|_|  \___/ 
;;                                   
;; Hand crafted                              

	        
PUBLIC _level0_map
._level0_map
        defb 0x10, 0x04         ; ink green
        defb 0x11, 0x07         ; paper white

        ;; Bottom row, with a blocker at each end
        
        defb 0x16, 0x16, 0x00   ; AT 22,0
        
        defb 0x80               ; grassh
        defb 0x0b               ; down
        defb 0x08               ; left

        defb 0x83               ; grassv in very bottom left corner
                
        defb 0x0e, 0x1e         ; repeat 30 times
        defb 0x80               ; grassh along the bottom
        defb 0x0f               ; end repeat

        defb 0x83               ; grassv in very bottom right corner
        defb 0x16, 0x16, 0x1f   ; up doesn't work, so AT 22,31
        defb 0x80               ; grassh


	;; Step 1
	
        defb 0x16, 0x15, 0x09   ; AT 21,9

	defb 0x0e, 0x6          ; repeat 6 times
        defb 0x80               ; grassh
        defb 0x0f               ; end repeat

        defb 0x16, 0x13, 0x04   ; AT 19,4
        defb 0x80               ; grassh
        defb 0x80               ; grassh
	

	;; Step 2
	
        defb 0x16, 0x12, 0x0A   ; AT 18,10

	defb 0x0e, 0x5          ; repeat 5 times
        defb 0x80               ; grassh
        defb 0x0f               ; end repeat


	;;  Single jump
	
        defb 0x16, 0x10, 0x13   ; AT 16,19
        defb 0x80               ; grassh
				

	;; Step 3
			
        defb 0x16, 0x0E, 0x17   ; AT 14,27
	
	defb 0x0e, 0x4          ; repeat 4 times
        defb 0x80               ; grassh
        defb 0x0f               ; end repeat
	

	;; Rest
	
        defb 0x16, 0x0D, 0x0A   ; AT 13,10
		
        defb 0x83               ; grassv
                
        defb 0x0e, 0x08         ; repeat 8 times
        defb 0x80               ; grassh
        defb 0x0f               ; end repeat

        defb 0x83               ; grassv

        defb 0x16, 0x0C, 0x0A   ; AT 12,10
        defb 0x80               ; grassh
        defb 0x16, 0x0C, 0x13   ; AT 12,10
        defb 0x80               ; grassh

        
        ;; Steps 4 and 5
        
        defb 0x16, 0x0B, 0x05   ; AT 11,5
        defb 0x80               ; grassh
        defb 0x80               ; grassh
        defb 0x80               ; grassh
        defb 0x9                ; right one
        defb 0x80               ; grassh

        defb 0x16, 0x09, 0x03   ; AT 9,4
        defb 0x80               ; grassh
        defb 0x80               ; grassh
        defb 0x80               ; grassh

        
        ;; Step 6 (long with fall at end)

        defb 0x16, 0x07, 0x0A   ; AT 7,10
		
        defb 0x0e, 0x0B         ; repeat 11 times
        defb 0x80               ; grassh
        defb 0x0f               ; end repeat

        
        ;; Step 7
        
        defb 0x16, 0x0A, 0x15   ; AT 10,21
        defb 0x80               ; grassh
        defb 0x80               ; grassh

        
        ;; Single grass & jumper step
        
        defb 0x16, 0x08, 0x1A   ; AT 8,26
        defb 0x80               ; grassh
        

        ;;  Bottom of wall

        defb 0x16, 0x07, 0x1C   ; AT 7,28
        defb 0x80               ; grassh
        defb 0x80               ; grassh
        defb 0x9                ; right
        defb 0x83               ; grassv

        ;; Build the wall. Left then up doesn't work at the edge of
        ;; the screen, so place them
        
        defb 0x16, 0x06, 0x1F   ; AT 6,31
        defb 0x83               ; grassv
        defb 0x16, 0x05, 0x1F   ; AT 5,31
        defb 0x83               ; grassv
        defb 0x16, 0x04, 0x1F   ; AT 4,31
        defb 0x83               ; grassv                
        defb 0x16, 0x03, 0x1F   ; AT 3,31
        defb 0x80               ; grassh
        		
        
        ;; Long top run

        defb 0x16, 0x04, 0x0B   ; AT 4,11
        
        defb 0x0e, 0x0C         ; repeat 13 times
        defb 0x80               ; grassh
        defb 0x0f               ; end repeat


        ;; Shaped final hurdle

        defb 0x16, 0x04, 0x03   ; AT 4,3
        defb 0x80               ; grassh
        defb 0x80               ; grassh
        defb 0x83               ; grassv
        defb 0x80               ; grassh
        defb 0x80               ; grassh
        defb 0x80               ; grassh
        defb 0x16, 0x03, 0x05   ; AT 3,5
        defb 0x80               ; grassv

        
        ;; Finish

        defb 0x16, 0x10, 0x00   ; AT 16,0
        defb 0x80               ; grassh
        defb 0x80               ; grassh
        defb 0x80               ; grassh

        
        
        ;; Now go back and fill in the jumpers

        defb 0x10, 0x02         ; ink red
        defb 0x11, 0x04         ; paper green
        
        defb 0x16, 0x17, 0x11   ; AT 23,17, centre of bottom row
        defb 0x81               ; jumper

        defb 0x16, 0x15, 0x08   ; AT 21,8, step 1
        defb 0x81               ; jumper

        defb 0x16, 0x13, 0x06   ; AT 19,6, step 2
        defb 0x81               ; jumper
                
        defb 0x16, 0x12, 0x0F   ; AT 18,15, step 3
        defb 0x81               ; jumper

        defb 0x16, 0x10, 0x14   ; AT 16,20, little step
        defb 0x81               ; jumper

        defb 0x16, 0x0E, 0x16   ; AT 14,22, onto rest
        defb 0x81               ; jumper

        defb 0x16, 0x0D, 0x0B   ; AT 13,11, escape from rest
        defb 0x81               ; jumper

        defb 0x16, 0x0B, 0x08   ; AT 11,8, step 4
        defb 0x81               ; jumper

        defb 0x16, 0x09, 0x06   ; AT 9,6, step 5
        defb 0x81               ; jumper

        defb 0x16, 0x0A, 0x17   ; AT 10,23, step 7
        defb 0x81               ; jumper

        defb 0x16, 0x08, 0x1B   ; AT 8,27, step 7
        defb 0x81               ; jumper
                                        
        defb 0x16, 0x07, 0x1E   ; AT 7,30, bottom of wall
        defb 0x81               ; jumper

        defb 0x16, 0x05, 0x1A   ; AT 5,26, isolated hop
        defb 0x81               ; jumper

        defb 0x16, 0x04, 0x0A   ; AT 4,10, final hurdle
        defb 0x81               ; jumper
        
        ;; Place the finish point

        defb 0x10, 0x06         ; ink yellow
        defb 0x11, 0x01         ; paper blue
        
        defb 0x16, 0x0F, 0x0    ; AT 15,0
        defb 0x82               ; finish
                
        defb 0x00
PUBLIC _level0_map_end
._level0_map_end





;;    _                    _   __ 
;;   | |                  | | /_ |
;;   | |     _____   _____| |  | |
;;   | |    / _ \ \ / / _ | |  | |
;;   | |___|  __/\ V |  __| |  | |
;;   |______\___| \_/ \___|_|  |_|
;;                                
;; Level designer

	
PUBLIC _level1_map
._level1_map

INCLUDE "level1_map.inc.asm"	
			        
        ;; Place the finish point

        defb 0x10, 0x06         ; ink yellow
        defb 0x11, 0x01         ; paper blue
        
        defb 0x16, 0x10, 0x0    ; AT 16,0
        defb 0x82               ; finish
		                
        defb 0x00
PUBLIC _level1_map_end
._level1_map_end




	
;;    _                    _   ___  
;;   | |                  | | |__ \ 
;;   | |     _____   _____| |    ) |
;;   | |    / _ \ \ / / _ | |   / / 
;;   | |___|  __/\ V |  __| |  / /_ 
;;   |______\___| \_/ \___|_| |____|
;;                                  
;; Level designer                                 


PUBLIC _level2_map
._level2_map

INCLUDE "level2_map.inc.asm"

        ;; Place the finish point

        defb 0x10, 0x06         ; ink yellow
        defb 0x11, 0x01         ; paper blue
        
        defb 0x16, 0x16, 0x1E   ; AT 22,30
        defb 0x82               ; finish
        
	defb 0x00
	
PUBLIC _level2_map_end
._level2_map_end





;;    _                    _   ____
;;   | |                  | | |___ \
;;   | |     _____   _____| |   __) |
;;   | |    / _ \ \ / / _ | |  |__ <
;;   | |___|  __/\ V |  __| |  ___) |
;;   |______\___| \_/ \___|_| |____/
;;
;; Level designer


PUBLIC _level3_map
._level3_map

INCLUDE "level3_map.inc.asm"

        ;; Place the finish point

        defb 0x10, 0x06         ; ink yellow
        defb 0x11, 0x01         ; paper blue

        defb 0x16, 0x01, 0x01   ; AT 1,1
        defb 0x82               ; finish

	defb 0x00

PUBLIC _level3_map_end
._level3_map_end





;;    _                    _   _  _
;;   | |                  | | | || |
;;   | |     _____   _____| | | || |_
;;   | |    / _ \ \ / / _ | | |__   _|
;;   | |___|  __/\ V |  __| |    | |
;;   |______\___| \_/ \___|_|    |_|
;;
;; Level designer


PUBLIC _level4_map
._level4_map

INCLUDE "level4_map.inc.asm"

        ;; Place the finish point

        defb 0x10, 0x06         ; ink yellow
        defb 0x11, 0x01         ; paper blue

        defb 0x16, 14, 1        ; AT side
        defb 0x82               ; finish

	defb 0x00

PUBLIC _level4_map_end
._level4_map_end
