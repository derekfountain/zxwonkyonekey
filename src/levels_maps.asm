SECTION rodata_user

;;  Paper is set using 3 bit colour values, i.e. the same as INK
;;  PrintString engine is about 730 bytes, so quite expensive

                
PUBLIC _level1_map
._level1_map
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
	
		        
        defb 0x00
PUBLIC _level1_map_end
._level1_map_end
