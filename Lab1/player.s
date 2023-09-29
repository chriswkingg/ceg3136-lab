
  AREA    |.text|, CODE, READONLY
  PRESERVE8;
  THUMB;


  EXPORT update_points_s

update_points_s FUNCTION	; char update_points_s(player_t *player, uint8_t target_info);
	; r0 *player
	; r1 target_info	
		PUSH          {r4-r7,lr};
		MOV           r2,r0;

		MOVS          r3,#0x00;
   
		LDRH          r0,[r2,#0x00];
		ADDS          r0,r0,#1;
		STRH          r0,[r2,#0x00];
    
		AND           r0,r1,#0xFC;
		CBZ           r0,l11;
   
		AND           r0,r1,#0x02;
		CBNZ          r0,l11;
    
		LDRH          r5,[r2,#0x02];
		ADDS          r5,r5,#1;
		STRH          r5,[r2,#0x02];
   
		UBFX          r0,r1,#2,#3;
    
		UBFX          r4,r1,#5,#2;
   
		SUBS          r5,r0,#1;
		ADDS          r6,r2,#6;
		ADD           r5,r6,r5,LSL #2;
		LDRB          r5,[r5,r4];
		ADDS          r5,r5,#1;
		UXTB          r7,r5;
		SUBS          r5,r0,#1;
		ADD           r5,r6,r5,LSL #2;
		STRB          r7,[r5,r4];
    
		ADDS          r3,r3,#1;
    
		SUBS          r5,r0,#1;
		ADD           r5,r6,r5,LSL #2;
		LDRB          r5,[r5,r4];
		CMP           r5,r0;
		BNE           l10;

		ADD           r3,r3,r0;

l10     NOP           ;
    
l11     LDRH          r0,[r2,#0x04];
		ADD           r0,r0,r3;
		STRH          r0,[r2,#0x04];
    
		MOV           r0,r3;
    
		POP           {r4-r7,pc};
		MOV           r0,#0x3000000;
		VMSR          FPSCR, r0;
		BX            lr;
		MOVS          r0,r0;
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
		PUSH          {r4-r5,lr};
		MOV           r3,r0;
		MOVS          r0,#0x01;
		MOVS          r1,#0x00;
		B             l5;
l1 		MOVS          r2,#0x00;
		B             l6;
l4 		ADDS          r4,r3,#6;
		ADD           r4,r4,r1,LSL #2;
		LDRB          r5,[r4,r2];
		ADDS          r4,r1,#1;
		CMP           r5,r4;
		BGE           l2;
		MOVS          r0,#0x00;
		B             l3;
l2   	ADDS          r2,r2,#1;
l6 		RSB           r4,r1,#0x04;
		CMP           r4,r2;
		BGT           l4 ;
l3      NOP           
		ADDS          r1,r1,#1;
l5      CMP           r1,#0x04;
		BLT           l1;
		POP           {r4-r5,pc};
  ENDFUNC
  
  END