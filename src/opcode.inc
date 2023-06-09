
macro opcode length,cycles,routine
	db cycles
	db length 
	emit 3: routine
end macro 


block_cycles_addr: 
	emit 3: 0 

max_cycles := 150

	;call emit_header 
; ix = input 
; iy = output 
opcode_gen_loop:
	add ix,bc
	
	exx 
	lea hl,ix+0 		
	or a,a 
	sbc hl,de 			
	jq z,.page_boundary
	lea hl,iy+0  
	ld de,$D40000 
	or a,a 
	sbc hl,de 
	jq nc,0 ;jit_flush
	exx 
	
	ld a,(ix+0) 
	; other opcode tables
	cp a,$CB
	jq z,gen_bit 
	cp a,$DD
	jq z,gen_ix
	cp a,$ED
	jq z,gen_extend
	cp a,$FD
	jq z,gen_iy 
	; find entry 
	ld l,a 
	ld h,5 
	ld de,opcode_table
	mlt hl 
	add hl,de
	ex af,af' 
	add a,(hl) 
	cp a,max_cycles 
	jq nc,out_of_cycles 
	ex af,af' 
	inc hl 
	ld c,(hl)
	inc hl 
	ld hl,(hl) 
	call call_hl
	or a,a 
	jq z,opcode_gen_loop
	
.page_boundary: 

gen_bit: 
gen_ix: 
gen_extend:
gen_iy:

out_of_cycles:
	
call_hl: 
	jp (hl)
	

opcode_table: 
	;0
	opcode 1,4,OP_NOP
	opcode 3,10,OP_BC
	opcode 1,7,OP_BC_WRITE
	opcode 1,6,OP_BC 
	opcode 1,4,OP_BC
	opcode 1,4,OP_BC
	opcode 2,7,OP_BC
	opcode 1,4,OP_COPY 
	
	;8 
	opcode 1,4,OP_EX_AF 
	opcode 1,11,OP_ADDHLBC 
	opcode 1,7,OP_BC_READ 
	opcode 1,6,OP_BC 
	opcode 1,4,OP_BC
	opcode 1,4,OP_BC
	opcode 2,7,OP_BC
	opcode 1,4,OP_COPY
	
	;10 
	
	opcode 2,13,OP_DJNZ 
	opcode 3,10,OP_DE 
	opcode 1,7,OP_BC_WRITE 
	opcode 1,6,OP_DE 
	opcode 1,4,OP_DE
	opcode 1,4,OP_DE
	opcode 2,7,OP_DE
	opcode 1,4,OP_COPY 
	
	;18 
	opcode 2,12,OP_JR 
	opcode 1,11,OP_ADDHLDE 
	opcode 1,7,OP_BC_READ 
	opcode 1,6,OP_BC 
	opcode 1,4,OP_BC
	opcode 1,4,OP_BC
	opcode 2,7,OP_BC
	opcode 1,4,OP_COPY
	
	;20 
	opcode 2,12,OP_JR_COND 
	opcode 3,10,OP_HL 
	opcode 3,16,OP_WRITE_ABS_HL 
	opcode 1,6,OP_HL 
	opcode 1,4,OP_HL
	opcode 1,4,OP_HL
	opcode 2,7,OP_HL
	opcode 1,4,OP_COPY
	
	;28 
	opcode 2,12,OP_JR_COND 
	opcode 1,11,OP_ADDHLHL 
	opcode 3,16,OP_READ_ABS_HL 
	opcode 1,6,OP_HL 
	opcode 1,4,OP_HL
	opcode 1,4,OP_HL
	opcode 2,7,OP_HL
	opcode 1,4,OP_COPY
	
	;30 
	opcode 2,12,OP_JR_COND 
	opcode 3,10,OP_SP 
	opcode 3,13,OP_WRITE_ABS_A 
	opcode 1,6,OP_SP 
	opcode 1,11,OP_IND_HL_RW
	opcode 1,11,OP_IND_HL_RW
	opcode 2,10,OP_WRITE_HL
	opcode 1,4,OP_COPY
	
	;38 
	opcode 2,12,OP_JR_COND 
	opcode 1,11,OP_ADDHLSP 
	opcode 3,13,OP_READ_ABS_A 
	opcode 1,6,OP_SP 
	opcode 1,4,OP_COPY
	opcode 1,4,OP_COPY
	opcode 2,7,OP_COPY
	opcode 1,4,OP_COPY
	
	;40 
	opcode 1,4,OP_NOP 
	opcode 1,4,OP_BC 
	opcode 1,4,OP_MOVETO_BC
	opcode 1,4,OP_MOVETO_BC
	opcode 1,4,OP_MOVETO_BC
	opcode 1,4,OP_MOVETO_BC
	opcode 1,7,OP_MOVETO_BC_HL 
	opcode 1,4,OP_BC
	
	;48 
	opcode 1,4,OP_BC 
	opcode 1,4,OP_NOP 
	opcode 1,4,OP_MOVETO_BC
	opcode 1,4,OP_MOVETO_BC
	opcode 1,4,OP_MOVETO_BC
	opcode 1,4,OP_MOVETO_BC
	opcode 1,7,OP_MOVETO_BC_HL
	opcode 1,4,OP_BC
	
	;50 
	opcode 1,4,OP_MOVETO_DE
	opcode 1,4,OP_MOVETO_DE
	opcode 1,4,OP_NOP
	opcode 1,4,OP_MOVETO_DE
	opcode 1,4,OP_MOVETO_DE
	opcode 1,4,OP_MOVETO_DE
	opcode 1,7,OP_MOVETO_DE
	opcode 1,4,OP_MOVETO_DE
	
	
	;58 
	opcode 1,4,OP_MOVETO_DE
	opcode 1,4,OP_MOVETO_DE
	opcode 1,4,OP_MOVETO_DE
	opcode 1,4,OP_NOP
	opcode 1,4,OP_MOVETO_DE
	opcode 1,4,OP_MOVETO_DE
	opcode 1,7,OP_MOVETO_DE
	opcode 1,4,OP_MOVETO_DE
	
	
	;60 
	opcode 1,4,OP_MOVETO_HL
	opcode 1,4,OP_MOVETO_HL
	opcode 1,4,OP_MOVETO_HL
	opcode 1,4,OP_MOVETO_HL
	opcode 1,4,OP_NOP
	opcode 1,4,OP_MOVETO_HL
	opcode 1,7,OP_MOVETO_HL
	opcode 1,4,OP_MOVETO_HL
	
	
	;68
	opcode 1,4,OP_MOVETO_HL
	opcode 1,4,OP_MOVETO_HL
	opcode 1,4,OP_MOVETO_HL
	opcode 1,4,OP_MOVETO_HL
	opcode 1,4,OP_MOVETO_HL
	opcode 1,4,OP_NOP
	opcode 1,7,OP_MOVETO_HL
	opcode 1,4,OP_MOVETO_HL
	
	
	;70 
	opcode 1,7,OP_IND_HL_WRITE
	opcode 1,7,OP_IND_HL_WRITE
	opcode 1,7,OP_IND_HL_WRITE
	opcode 1,7,OP_IND_HL_WRITE
	opcode 1,7,OP_IND_HL_WRITE
	opcode 1,4,OP_HALT
	opcode 1,7,OP_IND_HL_WRITE
	
	;78 
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,7,OP_ALU
	opcode 1,4,OP_NOP
	
	;80
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,7,OP_ALU
	opcode 1,4,OP_ALU
	
	;88
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,7,OP_ALU
	opcode 1,4,OP_ALU
	
	;90
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,7,OP_ALU
	opcode 1,4,OP_ALU
	
	;98
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,7,OP_ALU
	opcode 1,4,OP_ALU
	
	;A0
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,7,OP_ALU
	opcode 1,4,OP_ALU
	
	;A8
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,7,OP_ALU
	opcode 1,4,OP_ALU
	
	;B0
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,7,OP_ALU
	opcode 1,4,OP_ALU
	
	;B8
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,4,OP_ALU
	opcode 1,7,OP_ALU
	opcode 1,4,OP_ALU
	
	;C0 
	opcode 1,11,OP_RET_COND 
	opcode 1,10,OP_POP 
	opcode 3,10,OP_JP_COND 
	opcode 3,10,OP_JUMP 
	opcode 3,17,OP_CALL_COND 
	opcode 1,11,OP_PUSH  
	opcode 2,7,OP_COPY
	opcode 1,11,OP_RST 
	
	;C8
	opcode 1,11,OP_RET_COND 
	opcode 1,10,OP_RET  
	opcode 3,10,OP_JP_COND 
	opcode 2,0,OP_NULL 			; bit instructions  
	opcode 3,17,OP_CALL_COND
	opcode 3,17,OP_CALL 
	opcode 2,7,OP_COPY			 
	opcode 1,11,OP_RST
	
	;D0 
	opcode 1,11,OP_RET_COND 
	opcode 1,10,OP_POP 
	opcode 3,10,OP_JP_COND 
	opcode 3,10,OP_OUT_IMM 
	opcode 3,17,OP_CALL_COND 
	opcode 1,11,OP_PUSH 
	opcode 2,7,OP_COPY
	opcode 1,11,OP_RST 
	
	;D8
	opcode 1,11,OP_RET_COND 
	opcode 1,4,OP_EXX  
	opcode 3,10,OP_JP_COND 
	opcode 2,11,OP_IN_IMM
	opcode 3,17,OP_CALL_COND
	opcode 0,0,OP_NULL			; IX  
	opcode 2,7,OP_COPY
	opcode 1,11,OP_RST
	
	;E0 
	opcode 1,11,OP_RET_COND 
	opcode 1,10,OP_POP 
	opcode 3,10,OP_JP_COND 
	opcode 3,10,OP_EX_SP_HL 
	opcode 3,17,OP_CALL_COND 
	opcode 1,11,OP_PUSH 
	opcode 2,7,OP_COPY
	opcode 1,11,OP_RST
	
	;E8
	opcode 1,11,OP_RET_COND 
	opcode 1,4,OP_JP_HL   
	opcode 3,10,OP_JP_COND 
	opcode 2,11,OP_EX_DE_HL 
	opcode 3,17,OP_CALL_COND
	opcode 0,0,OP_NULL			; Extended   
	opcode 2,7,OP_COPY
	opcode 1,11,OP_RST
	
	
	;F0 
	opcode 1,11,OP_RET_COND 
	opcode 1,10,OP_POP 
	opcode 3,10,OP_JP_COND 
	opcode 3,10,OP_DI 
	opcode 3,17,OP_CALL_COND 
	opcode 1,11,OP_PUSH 
	opcode 2,7,OP_COPY
	opcode 1,11,OP_RST
	
	;F8
	opcode 1,11,OP_RET_COND 
	opcode 1,4,OP_LD_SP_HL   
	opcode 3,10,OP_JP_COND 
	opcode 2,11,OP_EI  
	opcode 3,17,OP_CALL_COND
	opcode 0,0,OP_NULL			; IY   
	opcode 2,7,OP_COPY
	opcode 1,11,OP_RST
	
	