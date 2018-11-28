SECTION MUSIC
ORG 0xCC00

    DEFC _F = 128
    DEFC _G = 114
    DEFC _A = 102
    DEFC _B =  96
    DEFC _C =  86
    DEFC _D =  80 ; ?
    DEFC _E =  76 ; ?

notes:
 	DEFB 128,114,102,96,86,102,86,86,81,96,81,81,86,102,86,86
 	;      F   G   A  B  C   A  CCCC #B ?G  ?BBB  C   A  CCCC
 	DEFB 128,114,102,96,86,102,86,86,81,96,81,81,86,86,86,86
 	DEFB 128,114,102,96,86,102,86,86,81,96,81,81,86,102,86,86
 	DEFB 128,114,102,96,86,102,86,64,86,102,128,102,86,86,86,86
    ;                              F

; 	DEFB _G,_G, _A, _G, _E         ; Lit-tle box-es
; 	DEFB _G,_G, _A, _G, _E         ; on the hill-side
; 	DEFB _G,_G, _G, _C, _C         ; Lit-tle box-es
;	DEFB _C,_C, _D, _E, _D, _C, _C ; ma-de of ticky tacky

;	DEFB _C,_C, _A, _G, _G         ; Lit-tle box-es
;	DEFB _G,_G, _A, _F, _F         ; Lit-tle box-es
;	DEFB _F,_F, _G, _E, _E         ; Lit-tle box-es
;	DEFB _E,_E, _F, 64, 64         ; All the same

;   DEFB _C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C,_C

; 	DEFB 114,114,102, 114,102, 114,114,102, 86,86, 74,74,86,86, 102,102,102,102
; 	DEFB 114,114,102,102, 114,114,102,102, 114,114,102,102, 86,86,86,86, 74,74,86,86, 102,102,102,102
; 	DEFB 114,114,102,102, 114,114,102,102, 114,114,102,102, 86,86,86,86, 74,74,86,86, 102,102,102,102
; 	DEFB 114,114,102,102, 114,114,102,102, 114,114,102,102, 86,86,86,86, 74,74,86,86, 102,102,102,102
; 	DEFB 114,114,102,102, 114,114,102,102, 114,114,102,102, 86,86,86,86, 74,74,86,86, 102,102,102,102
; 	DEFB 114,102,114,102, 114,102,114,102, 114,102,86,86, 74,86,102,102
; 	DEFB 114,102,114,102, 114,102,114,102, 114,102,86,86, 74,86,102,102
; 	DEFB 114,102,114,102, 114,102,114,102, 114,102,86,86, 74,86,102,102
; 	DEFB 114,102,114,102, 114,102,114,102, 114,102,86,86, 74,86,102,102

note_index:
	defb 0

PUBLIC _play_original_mm_note

_play_original_mm_note:
   pop af
   pop hl
   pop de
   pop bc

   push bc
   push de
   push hl
   push af

;ensure hl is pointing to parm

 	LD A,(note_index) ; 	Increment the in-game music note index at 33883
 	INC A
 	LD (note_index),A

        ;; Clear top and bottom bits of index value.
        ;; Rotate right to divide by 2 (LSB goes into top bit, so top bit stays 0)
        ;; Each note is played twice.
        
 	AND 126 	;Point HL at the appropriate entry in the tune data table at 34188
 	RRCA
 	LD E,A
 	LD D,0
 	LD HL,notes
 	ADD HL,DE

;34891 	LD A,(32883) 	Pick up the border colour for the current cavern from 32883
	ld a, 0

 	LD E,(HL) 	;Initialise the pitch delay counter in E
 	LD BC,3;3 	;Initialise the duration delay counters in B (0) and C (3)
o_out_loop:
 	OUT (254),A 	                   ;T=11 Produce a note of the in-game music
 	DEC E                              ;T=4
 	JR NZ,o_pitch_loop_not_done          ;T=12 if met, T=7 if not
 	LD E,(HL)                          ;T=7
	XOR 24                             ;T=7
o_pitch_loop_not_done:
 	DJNZ o_out_loop                      ;T=13 if not zero (i.e. it jumps), otherwise T=8
 	DEC C                              ;T=4
 	JR NZ,o_out_loop                     ;T=12 if met, T=7 if not

	ret

; E is from data - so 128 to start. B is 0, hence 256. It counts down E and B together doing the OUT, but not changing A. So no click.
; When E gets to zero it reloads E from data - still 128 - then flips the bit in A. B is still counting down, so it keeps looping.
; Next time round it clicks.
; So, E is pitch. It clicks every 'E' iterations round the loop.
; BC is 256*3 (768). That's how long the note lasts.
; Each note is the same length - 768 iterations of the loop.
; With C=3, so 768 iterations, it almost matches the original with no workload in wonky, and 4 halts. SO that's one note every 200ms.


PUBLIC _play_mm_note

_play_mm_note:
   pop af
   pop hl
   pop de
   pop bc

   push bc
   push de
   push hl
   push af

	ld a, 0

    ld d,0
 	LD E,(HL) 	;Initialise the pitch delay counter in E
 	LD BC,3;3 	;Initialise the duration delay counters in B (0) and C (3)
out_loop:
 	OUT (254),A 	                   ;T=11 Produce a note of the in-game music
 	DEC E                              ;T=4
 	JR NZ,pitch_loop_not_done          ;T=12 if met, T=7 if not
 	LD E,(HL)                          ;T=7
	XOR 24                             ;T=7
pitch_loop_not_done:
 	DJNZ out_loop                      ;T=13 if not zero (i.e. it jumps), otherwise T=8
 	DEC C                              ;T=4
 	JR NZ,out_loop                     ;T=12 if met, T=7 if not

	ret
