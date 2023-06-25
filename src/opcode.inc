
max_cycles := 150

macro opcode length,cycles,routine
	db cycles
	db length 
	emit 3: routine
end macro 

macro emit_code label 
	ld hl,label 
	lea de,iy+0 
	push bc
	ld bc,label.len 
	add iy,bc 
	ldir 
	pop bc
	xor a,a 
	ret 
end macro 

macro z80_start label 
assume adl = 0 
label: 
end macro 

macro z80_end label 
assume adl = 1 
label.len:= $ - label 
end macro



section .bss

public block_start_addr
public block_cycles_addr
public block_z80_addr
public block_cache_addr
public block_next_split


block_start_addr: 
	rb 3 
block_cycles_addr: 
	rb 3  
block_z80_addr: 
	rb 3
block_cache_addr: 
	rb 3
block_next_split:
	rb 3
	
section .text 
assume adl = 1 

start_block:
	ld (block_start_addr),ix
	lea hl,iy+2
	ld (block_cycles_addr),hl 
	ld de,(block_z80_addr)
	ld hl,block_header_address
	ld (hl),e 
	inc hl 
	ld (hl),d
	
	ld hl,block_header_code
	lea de,iy+0 
	ld bc,block_header_code_len
	add iy,bc 
	ldir 
	
	xor a,a 
	ex af,af'
	
	ret 
end_block:
	ex af,af' 
	ld hl,(block_cycles_addr) 
	ld (hl),a 
	lea hl,ix+0 
	ld de,(block_start_addr) 
	or a,a 
	sbc hl,de
	ex de,hl
	ld hl,(block_z80_addr)
	add hl,de 
	ld (block_z80_addr),hl
	ld (block_cache_addr),iy 
	
	ret 
	
block_header_code: 
assume adl=0 
	ex af,af' 
	sub a,0
block_header_cycles:= $-1 
	jr nc,$+6 
	ld hl,0
block_header_address:= $-2
	rst rst_line 
	ex af,af' 
assume adl=1 
block_header_code_len:= $ - block_header_code

_jit_translate: 
jit_translate: 
	push ix 
	pop de
	ld hl,$FFFFFF
	ld (block_next_split),hl 
	inc hl 
	ld (block_z80_addr),hl 
	pop hl 
	pop iy 
	pop ix 
	push hl 
	push hl 
	push hl
	push de 
	call start_block 
	
; ix = input 
; iy = output 
; bc = 0 
opcode_gen_loop:
	exx 
	lea de,ix+0	
	ld hl,(block_next_split) 
	or a,a 
	sbc hl,de
	jq z,block_split
	call c,load_split
	lea hl,iy+0  
	ld de,$D40000 
	or a,a 
	sbc hl,de 
	jp nc,jit_flush
	exx 
	
	ld a,(ix+0) 
	; other opcode tables
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
	add ix,bc
.test_return:
	or a,a 
	jq z,opcode_gen_loop
	cp a,1 
	jq z,.next_block
.end_meta_block: 
	call end_block 
	pop ix 
	ret 
.next_block: 
	call end_block 
	call start_block 
	jq opcode_gen_loop
	
call_hl: 
	jp (hl)
	
gen_ix: 
gen_extend:
gen_iy:
out_of_cycles:
block_split:
load_split:
	ret 





	

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
	opcode 1,4,MOVE_RR_BC
	opcode 1,4,MOVE_RR_BC
	opcode 1,4,OP_NOP
	opcode 1,4,MOVE_RR_DEHL
	opcode 1,4,MOVE_RR_DEHL
	opcode 1,4,MOVE_RR_DEHL
	opcode 1,7,OP_MOVETO_RR_IND_HL
	opcode 1,4,MOVE_RR_DEHL
	
	
	;58 
	opcode 1,4,MOVE_RR_BC
	opcode 1,4,MOVE_RR_BC
	opcode 1,4,MOVE_RR_DEHL
	opcode 1,4,OP_NOP
	opcode 1,4,MOVE_RR_DEHL
	opcode 1,4,MOVE_RR_DEHL
	opcode 1,7,OP_MOVETO_RR_IND_HL
	opcode 1,4,MOVE_RR_DEHL
	
	
	;60 
	opcode 1,4,MOVE_RR_BC
	opcode 1,4,MOVE_RR_BC
	opcode 1,4,MOVE_RR_DEHL
	opcode 1,4,MOVE_RR_DEHL
	opcode 1,4,OP_NOP
	opcode 1,4,MOVE_RR_DEHL
	opcode 1,7,OP_MOVETO_RR_IND_HL
	opcode 1,4,MOVE_RR_DEHL
	
	
	;68
	opcode 1,4,MOVE_RR_BC
	opcode 1,4,MOVE_RR_BC
	opcode 1,4,MOVE_RR_DEHL
	opcode 1,4,MOVE_RR_DEHL
	opcode 1,4,MOVE_RR_DEHL
	opcode 1,4,OP_NOP
	opcode 1,7,OP_MOVETO_RR_IND_HL
	opcode 1,4,MOVE_RR_DEHL
	
	
	;70 
	opcode 1,7,OP_IND_HL_WRITE_BC
	opcode 1,7,OP_IND_HL_WRITE_BC
	opcode 1,7,OP_IND_HL_WRITE
	opcode 1,7,OP_IND_HL_WRITE
	opcode 1,7,OP_IND_HL_WRITE
	opcode 1,7,OP_IND_HL_WRITE
	opcode 1,4,OP_HALT
	opcode 1,7,OP_WRITE_HL
	
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
	opcode 2,0,OP_BIT 			; bit instructions  
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
	

opcode_table_ix: 
	; 0 
	opcode 2,8,OP_NOP
	opcode 2,8,OP_NOP
	opcode 2,8,OP_NOP
	opcode 2,8,OP_NOP
	opcode 2,8,OP_IX_NOMAP
	opcode 2,8,OP_IX_NOMAP
	opcode 3,11,OP_IX_NOMAP
	opcode 2,8,OP_NOP
	
	;8 
	opcode 2,8,OP_NOP
	opcode 2,15,OP_ADDIXBC 
	opcode 2,8,OP_NOP
	opcode 2,8,OP_NOP
	opcode 2,8,OP_IX_NOMAP
	opcode 2,8,OP_IX_NOMAP
	opcode 3,11,OP_IX_NOMAP
	opcode 2,8,OP_NOP
	

; gather block end conditions

; marked by: 
	; out of cycles 
	; JP_COND/JR_COND/DJNZ  
	; RET/JP/CALL/RST (end of metablock) 
	; page boundary ( end of metablock )

emit_indhl: 
	emit_code ind_hl_code
z80_start ind_hl_code
	ex af,af'
	ld.lil hl,i 
	ld l,d  
	ld l,(hl) 
	inc h 
	ld.lil hl,(hl) 
	add.lil hl,de 
	ex af,af' 
z80_end ind_hl_code
	
emit_indix:
	ld (smc_displacement),a 
	emit_code ind_ix_code
z80_start ind_ix_code 
	ex af,af'
	push de 
	lea de,ix+0
smc_displacement:=$-1 
	ld.lil hl,i
	ld l,d 
	ld l,(hl) 
	inc h 
	ld.lil hl,(hl) 
	add.lil hl,de 
	pop de 
	ex af,af'
z80_end ind_ix_code
	
	
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; normal ops	

OP_NULL:
OP_NOP: 
	xor a,a 
	ret 
	
OP_COPY: 
	ld a,c 
	or a,a 
	ret z 
	lea de,iy+0 
	lea hl,ix+0 
	ldir 
	ld c,a
	add iy,bc
	xor a,a 
	ret 
	
OP_IX_NOMAP: 
	
OP_ALU: 
	and a,0111b 	; get register 
	cp a,2 
	jr c,.bc 
	cp a,6
	jr z,.indhl
	cp a,7 
	jr z,.copy 
.dehl: 
	ld a,(ix+0) 
	sub a,2 
	ld (iy+0),a
	inc iy 
	xor a,a 
	ret 
.bc: 
	ld a,$D9 			; exx
	ld (iy+0),a 
	ld (iy+2),a 
	ld a,(ix+0) 
	ld (iy+1),a 
	lea iy,iy+3 
	xor a,a 
	ret 
.indhl: 
	call emit_indhl 
	ld (iy+0),$5B		; .lil prefix
	ld a,(ix+0) 
	ld (iy+1),a 
	lea iy,iy+2 
	xor a,a 
	ret 
.copy: 
	ld a,(ix+0) 
	ld (iy+0),a 
	inc iy 
	xor a,a 
	ret 
	


OP_BIT:
	ex af,af' 	; base cc 
	add a,8 
	ex af,af' 
	ld a,(ix+1) 
	cp a,$30 
	jr c,.legal
	cp a,$39 
	jr c,gen_sll
.legal:
	and a,0111b		; find register 
	cp a,2 
	jr c,.bc 
	cp a,6 
	jr c,.dehl 
	jr z,.indhl 
	; bit a instructions 
	ld a,(ix+1) 
	ld (iy+0),$CB 
	ld (iy+1),a 
	lea iy,iy+2
	xor a,a 
	ret 
.bc: ; exx \ bit b/c \ exx 
	ld a,$D9 			; exx
	ld (iy+0),a
	ld (iy+3),a
	ld (iy+1),$CB 
	ld a,(ix+1) 
	ld (iy+2),a 
	lea iy,iy+4 
	xor a,a 
	ret 
.dehl: 
	ld (iy+0),$CB 
	ld a,(ix+1) 
	sub a,2
	ld (iy+1),a 
	lea iy,iy+2 
	xor a,a 
	ret 
.indhl: 
	ex af,af' 
	add a,7 
	ex af,af' 
	call emit_indhl
	ld (iy+0),$5B		; .lil prefix
	ld (iy+1),$CB
	ld a,(ix+1) 
	ld (iy+2),a 
	lea iy,iy+3 
	; check if bit b,r instruction 
	cp a,$40 
	jr c,.addrst 
	cp a,$80 
	jr nc,.addrst 
	xor a,a 
	ret
.addrst: 
	ld (iy+0),$DF		; rst $18 
	inc iy 
	xor a,a 
	ret 
	
	
	
gen_sll: 
	; sla => scf \ rla 
	ld (iy+0),$37		; scf 
	inc iy
	and a,0111b		; find register 
	cp a,2 
	jr c,.bc 
	cp a,6 
	jr c,.dehl 
	jr z,.indhl 
	; bit a instructions 
	ld a,(ix+1) 
	ld (iy+0),$CB 
	ld (iy+1),a 
	lea iy,iy+2
	xor a,a 
	ret 
.bc: ; exx \ bit b/c \ exx 
	ld a,$D9 			; exx
	ld (iy+0),a
	ld (iy+3),a
	ld (iy+1),$CB 
	ld a,(ix+1)
	sub a,$20
	ld (iy+2),a 
	lea iy,iy+4 
	xor a,a 
	ret 
.dehl: 
	ld (iy+0),$CB 
	ld a,(ix+1) 
	sub a,$22
	ld (iy+1),a 
	lea iy,iy+2 
	xor a,a 
	ret 
.indhl: 
	call emit_indhl
	ld (iy+0),$5B		; .lil prefix
	ld (iy+1),$CB
	ld a,(ix+1)
	sub a,$20
	ld (iy+2),a 
	ld (iy+3),$DF 		; rst $18
	lea iy,iy+4 
	xor a,a 
	ret

OP_POP: 
	ld (iy+0),$F7 		; rst $30 
	ld (iy+1),a
	lea iy,iy+2 
	xor a,a 
	ret 

OP_PUSH: 
	ld (iy+0),a 
	ld (iy+1),$EF			; rst $28 
	lea iy,iy+2 
	xor a,a 
	ret 

	
	; transforms bc ops into exx\op\exx 
OP_BC: 
	ld (iy+0),$D9	;exx 
	inc iy 
	call OP_COPY 
	ld (iy+0),$D9 
	inc iy 
	ret 
	
	; de => bc , hl=> de 
OP_HL:
OP_DE: 
	sub a,$10
	ld (iy+0),a 
	inc iy
	inc ix 
	dec c 
	call OP_COPY 
	ret 
	
	; sp => iy 
OP_SP: 
	ld (iy+0),$FD 	; iy prefix 
	inc iy 
	sub a,$10
	ld (iy+0),a 
	inc iy 
	dec c 
	call OP_COPY 
	inc c 
	ret 
	
OP_MOVETO_BC: 
	; push rr \ exx \ pop hl \ ld b,l \ exx 
	bit 1,a
	jr nz,.de 
.hl: 
	rra
	ld a,$D5		; push de 
	jr .routine 
.de: 
	rra
	ld a,$C5		; push bc
.routine: 
	ld (move_bc_code.smc_reg),a
	jr c,.lowbyte 
.highbyte:
	ld a,$44		; ld b,h 
	ld (move_bc_code.smc_hl),a 
	jr .emit
.lowbyte: 
	ld a,$4D		; ld c,l
	ld (move_bc_code.smc_hl),a
.emit: 
	emit_code move_bc_code 	
	
z80_start move_bc_code
.smc_reg:
	nop
	exx 
	pop hl 
.smc_hl:
	ld b,h 
	exx
z80_end move_bc_code

MOVE_RR_BC:
	sub a,$10 - 4		; shift register down one row and change to ld r,h or ld r,l 
	ld (move_rr_bc_code.smc_reg),a 
	emit_code move_rr_bc_code
	
z80_start move_rr_bc_code 
	exx 
	push bc 
	exx 
	pop hl 
.smc_reg: 	
	nop 
z80_end move_rr_bc_code

MOVE_RR_DEHL: 
	sub a,$12			; shift d/e to b/c and h/l to d/e 
	ld (iy+0),a
	inc iy
	xor a,a 
	ret 
	
	
	
OP_BC_WRITE:
	emit_code wbc_code 
z80_start wbc_code
	exx 
	call write_de 
	exx 
z80_end wbc_code

OP_BC_READ: 
	emit_code rbc_code
z80_start rbc_code
	exx 
	call read_de 
	exx 
z80_end rbc_code

OP_DE_WRITE:
	emit_code wde_code 
z80_start wde_code
	call write_de 
z80_end wde_code

OP_DE_READ: 
	emit_code rde_code
z80_start rde_code
	call read_de 
z80_end rde_code



OP_ADDHLBC: 
	emit_code addhlbc_code
z80_start addhlbc_code
	exx
	push bc 
	exx 
	pop hl 
	add hl,de 
	ex de,hl 
z80_end addhlbc_code


OP_ADDHLDE: 
	emit_code addhlde_code
z80_start addhlde_code
	ex de,hl 
	add hl,bc  
	ex de,hl 
z80_end addhlde_code

OP_ADDHLHL: 
	emit_code addhlhl_code
z80_start addhlhl_code
	ex de,hl 
	add hl,hl 
	ex de,hl 
z80_end addhlhl_code

OP_ADDHLSP: 
	emit_code addhlsp_code
z80_start addhlsp_code
	lea hl,iy+0 
	add hl,de 
	ex de,hl 
z80_end addhlsp_code

OP_EX_AF: 
	emit_code ex_af_code
z80_start ex_af_code
	call exchange_af 
z80_end ex_af_code

OP_EXX: 
	emit_code exx_code
z80_start exx_code 
	call exchange_af 
z80_end exx_code

OP_EX_SP_HL:
	emit_code ex_sp_hl_code
z80_start ex_sp_hl_code
	call exchange_sp_hl 
z80_end ex_sp_hl_code 

OP_EX_DE_HL: 
	emit_code ex_de_hl_code
z80_start ex_de_hl_code	
	ex de,hl 
	ld d,b 
	ld e,c 
	ld b,h 
	ld c,l 
z80_end ex_de_hl_code

OP_LD_SP_HL: 
	emit_code ld_sp_hl_code
z80_start ld_sp_hl_code 
	ld iyl,e 
	ld iyh,d
z80_end ld_sp_hl_code

OP_EI:
	emit_code ei_code 
z80_start ei_code
	ld hl,z80_int_enabled
	ld (hl),1
z80_end ei_code

OP_DI:
	emit_code di_code 
z80_start di_code
	ld hl,z80_int_enabled
	ld (hl),0
z80_end di_code




;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; (HL)

;instructions like `inc (hl)` 
OP_IND_HL_RW: 
	call emit_indhl 
	ld a,(ix+0) 
	ld (iy+0),a 
	ld (iy+1),$DF 		; rst $18 
	lea iy,iy+2 
	xor a,a 
	ret 

OP_MOVETO_RR_IND_HL: 
	call emit_indhl 
	ld a,(ix+0) 
	sub a,$10
	ld (iy+0),$5B 		; .lil prefix
	ld (iy+1),a 
	lea iy,iy+2 
	xor a,a 
	ret 

OP_MOVETO_BC_HL: 
	ld (moveto_bc_hl_code.smc_reg+1),a 
	emit_code moveto_bc_hl_code
	
z80_start moveto_bc_hl_code
	push de 
	exx 
	pop de 
	ex af,af'
	ld.lil hl,i 
	ld l,d  
	ld l,(hl) 
	inc h 
	ld.lil hl,(hl) 
	add.lil hl,de 
	ex af,af'
.smc_reg: 	
	ld.lil b,(hl) 
	exx 
z80_end moveto_bc_hl_code


OP_IND_HL_WRITE_BC: 
	ld (ind_hl_write_bc_code.smc_reg+1),a 
	emit_code ind_hl_write_bc_code

z80_start ind_hl_write_bc_code
	push de 
	exx 
	pop de 
	ex af,af'
	ld.lil hl,i 
	ld l,d  
	ld l,(hl) 
	inc h 
	ld.lil hl,(hl) 
	add.lil hl,de 
	ex af,af'
.smc_reg: 	
	ld.lil (hl),b
	rst rst_verify_write_hl
	exx 
z80_end ind_hl_write_bc_code


OP_IND_HL_WRITE: 
	call emit_indhl
	ld a,(ix+0) 
	sub a,2 
	ld (iy+0),$5B 
	ld (iy+1),a 
	ld (iy+2),$DF 
	lea iy,iy+3 
	xor a,a 
	ret 
	
OP_WRITE_HL: 
	call emit_indhl
	ld a,(ix+0) 
	ld (iy+0),$5B 
	inc iy 
	call OP_COPY 
	ld (iy+0),$DF 
	inc iy 
	xor a,a 
	ret 


;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;I/O 

; call mn \ ld a,l
OP_IN_IMM: 

; ld l,a \ push af \ call mn 
OP_OUT_IMM: 


;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;absolute read/write 

OP_READ_ABS_HL: 
OP_READ_ABS_A: 

OP_WRITE_ABS_HL:
OP_WRITE_ABS_A: 

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; branch 


; ld hl,addr / rst $00 
; OR 
; jp cond,block  

OP_JR_COND:
OP_DJNZ: 
OP_JP_COND: 
OP_CALL_COND:
OP_RET_COND: 
	ld a,1 
	ret 

OP_JR:
OP_JUMP: 
OP_JP_HL: 
OP_CALL: 
OP_RET:
OP_RST:
OP_HALT:
	ld a,$FF
	ret 

	