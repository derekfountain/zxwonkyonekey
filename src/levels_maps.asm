SECTION rodata_user

;;  In the SP1 print string routine, embedded paper colours are
;;  set using 3 bit colour values, i.e. the same as INK.
;;  Don't use the PAPER_* macro values here.

        


;;    _                    _    ___  
;;   | |                  | |  / _ \ 
;;   | |     _____   _____| | | | | |
;;   | |    / _ \ \ / / _ | | | | | |
;;   | |___|  __/\ V |  __| | | |_| |
;;   |______\___| \_/ \___|_|  \___/ 
;;                                   
;;                                   

	        
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

        defb 0x16, 0x09, 0x04   ; AT 9,4
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
;;                                

	
PUBLIC _level1_map
._level1_map
        defb 0x10, 0x07         ; ink white
        defb 0x11, 0x00         ; paper black

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

	
	;; Top run
	
        defb 0x16, 0x00, 0x00   ; AT 0,0
        
        defb 0x80               ; grassh
        defb 0x0b               ; down
        defb 0x08               ; left

        defb 0x83               ; grassv below
                
        defb 0x0e, 0x14         ; repeat 20 times
        defb 0x80               ; grassh
        defb 0x0f               ; end repeat
	
	
	;;  Step 1, after first initial fall

        defb 0x16, 0x11, 0x0F   ; AT 17,15

	defb 0x0e, 0x09         ; repeat
        defb 0x80               ; grassh
        defb 0x0f               ; end repeat

	;; Step 2, needs reverse jump to reach, teleporter at end
		        
        defb 0x16, 0x0F, 0x11   ; AT 15,17

	defb 0x0e, 0x04         ; repeat
        defb 0x80               ; grassh
        defb 0x0f               ; end repeat

	

	;; Top left step with teleporter at end
	
        defb 0x16, 0x04, 0x00   ; AT 4,0

	defb 0x0e, 0x06         ; repeat 6 times
        defb 0x80               ; grassh
        defb 0x0f               ; end repeat

	;; Teleports back down to left side platform:

	;;  Step x, first on left side

        defb 0x16, 0x11, 0x00   ; AT 17,0

	defb 0x0e, 0x08         ; repeat
        defb 0x80               ; grassh
        defb 0x0f               ; end repeat
	
	
        ;; Now go back and fill in the jumpers

        defb 0x10, 0x02         ; ink red
        defb 0x11, 0x07         ; paper white
        
        defb 0x16, 0x17, 0x07   ; AT 23,7, bottom row
        defb 0x81               ; jumper
	
        defb 0x16, 0x11, 0x0E   ; AT 17,14
        defb 0x81               ; useless bogus jumper

		        
        ;; Place the finish point

        defb 0x10, 0x06         ; ink yellow
        defb 0x11, 0x01         ; paper blue
        
        defb 0x16, 0x10, 0x0    ; AT 16,0
        defb 0x82               ; finish

	
	;; prime colour for teleporters which get filled in by the C code in
	;; print_level_from_sp1_string()
	
        defb 0x10, 0x04         ; ink green
        defb 0x11, 0x03         ; paper blue
	                
        defb 0x00
PUBLIC _level1_map_end
._level1_map_end
	