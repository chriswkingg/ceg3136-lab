
  AREA    |.text|, CODE, READONLY
  PRESERVE8;
  THUMB;


  EXPORT update_points_s

update_points_s FUNCTION	; char update_points_s(player_t *player, uint8_t target_info);
	; r0 *player
	; r1 target_info	
	MOV r2, r0					; move player pointer to r2
	MOV r3, #0x00				; points_urned = 0
	
    LDRH r0,[r2,#0x00]	;load value of nAttempts
	ADDS r0,r0,#1		; increment
    STRH r0,[r2,#0x00]	; store nAttempts
	
	AND  r0,r1,#0xFC	;
	CBZ  r0, endandreturn	;
	
	AND r0, r1, #0x02
	CBNZ r0, endandreturn	;
	
	LDRH r0,[r2,#0x02]	;load value of nHits
	ADDS r0,r0,#1		; increment
    STRH r0,[r2,#0x02]	; store nHits
	
	
	UBFX r0, r1, #0x02, #0x03 ; type is in r0
	
	UBFX r4, r1, #0x05, #0x02 ;indx is in r4
	
    SUBS r5,r0,#1			; type - 1 in r5
	ADDS r6,r2,#6			; player pieces hit offset
	ADD r5,r6,r5,LSL #2		; pieces hit + type-1 left shifted 2
	LDRB r5,[r5,r4]			; save pieces hit value to r5
	ADDS r5,r5,#1			; increment
	UXTB r7,r5				; removes zeros???
	SUBS r5,r0,#1			; subtract 1 from incremented pieces hit value
	ADD r5,r6,r5,LSL #2		; recalculate pieces hit address
	STRB r7,[r5,r4]			; store new value at same address
	
	ADDS r3,r3, #0x01		; increment points_urned
	
	SUBS r5, r0, #0x01		;
	ADD r5, r6, r5, LSL #0x02;
	LDRB r5, [r5,r4]		; load pieces_hit value into r5
	CMP r5, r0				; compare pieces_hit to type
	BNE endandreturn
	
	ADD r3, r3, r0			; points_urned += type
	
endandreturn	LDRH r0, [r2,#0x04]		;load value of points
	ADD r0, r0, r3			; add points_urned to player.points
	STRH r0, [r2,#0x04]		; save it
	
	MOV r0, r3				; move points_urned to return value 
	BX	LR						; return 0
  ENDFUNC
  

  EXPORT player_get_num_pieces_destroyed_s

player_get_num_pieces_destroyed_s FUNCTION	; int  player_get_num_pieces_destroyed_s (player_t * player);
	; r0 *player
	PUSH {r4-r5,lr}			;
	MOV R0, R3				; move player pointer to r3
	MOV R0, #0				; n
	MOV R1, #0				; i
	MOV R2, #0				; j	
loop1A	CMP R1, #0x04		; if i >= 4, end
		BGE endloop1A		;

loop2A	CMP R2, #0x04	;
			BGE endloop2A	; if j >= 4, end
			ADDS r4, r3, #0x06		;
			ADD r4, r4, r1, LSL #0x02	; calculate array offset
			LDRB r5, [r4, r2]	; get value at calculated address
			ADDS r4, r1, #0x01	; 
			
			CMP r5, r4		; if not equal continue
			BNE noIncN	;
			ADDS r0, #0x01	; n++
			
noIncN		ADDS R2, #0x01	; j++
			B loop2A		; back to start of inner loop
			
endloop2A	ADDS R1, #0x01		; i++
			MOV r2, #0x00		; j = 0
			B loop1A			; back to start of outter loop
endloop1A	POP{r4-r5, pc}
			BX	LR					; return n
  ENDFUNC
  
  EXPORT player_done_s

player_done_s FUNCTION	; char player_done_s(player_t * player);
	; r0 *player
	MOV R0, R3				; move player pointer to r3
	MOV R0, #0x01				; won
	MOV R1, #0x00				; i
	MOV R2, #0x00				; j	
loop1B	CMP R0, #0x04		; if i >= 4, end
		BGE endloop1B		;

loop2B	CMP R1, #0x04	;
			BGE endloop2B	; if j >= 4, end
			
			ADDS r4, r3, #0x06		;
			ADD r4, r4, r1, LSL #0x02	; calculate array offset
			LDRB r5, [r4, r2]	; get value at calculated address
			ADDS r4, r1, #0x01	; 
			
			CMP r5, r4		; if greater than, return
			BGE endloop1B	;
			ADDS R2, #0x01	; j++
			B loop2B		; back to inner loop
endloop2B 	ADDS R1, #0x01		; i++
			B loop1B			; back to outter loop
endloop1B	BX	LR					; return n
  ENDFUNC
  
  END