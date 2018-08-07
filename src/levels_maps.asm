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
                
        defb 0x0e, 0x1e         ; repeat 30 times:      
        defb 0x80               ; grassh along the bottom
        defb 0x0f               ; end repeat

        defb 0x83               ; grassv in very bottom right corner
        defb 0x16, 0x16, 0x1f   ; up doesn't work, so AT 22,31
        defb 0x80               ; grassh
        
        defb 0x00
PUBLIC _level1_map_end
._level1_map_end
