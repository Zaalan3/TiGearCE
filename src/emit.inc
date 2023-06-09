


; gather block end conditions

; marked by: 
	; out of cycles 
	; JP_COND/JR_COND/DJNZ  
	; RET/JP/CALL/RST (end of metablock) 
	; page boundary ( end of metablock )
	
assume adl = 1 
	
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


emit_indhl: 
	emit_code ind_hl_code
ind_hl_code: 
	assume adl=0 
	ex af,af'
	ld.lil hl,i 
	ld l,d  
	ld l,(hl) 
	inc h 
	ld.lil hl,(hl) 
	add.lil hl,de 
	ex af,af' 
	assume adl=1 
ind_hl_code.len := $ - ind_hl_code 
	
emit_indix:
	ld (smc_displacement),a 
	emit_code ind_ix_code
ind_ix_code: 
	assume adl=0
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
	assume adl=1 
ind_ix_code.len := $ - ind_ix_code 
	
	
	


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
	dec c 
	call OP_COPY 
	inc c 
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
	
	
OP_MOVETO_BC: 
	; push rr \ exx \ pop hl \ ld b,l \ exx 
	bit 1,a
	jr nz,.de 
.hl: 
	rra
	ld a,$D1		; push de 
	jr .routine 
.de: 
	rra
	ld a,$C1		; push bc
.routine: 
	ld (move_bc_code),a
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
	
	assume adl = 0
move_bc_code:
	nop
	exx 
	pop hl 
.smc_hl:
	ld b,h 
	exx
	assume adl = 1 
move_bc_code.len := $ - move_bc_code
	

	
	
OP_BC_WRITE:
	emit_code wbc_code 
wbc_code: 
	assume adl=0
	exx 
	call write_de 
	exx 
	assume adl=1 
wbc_code.len := $ - wbc_code 

OP_BC_READ: 
	emit_code rbc_code
rbc_code: 
	assume adl=0
	exx 
	call read_de 
	exx 
	assume adl=1 
rbc_code.len := $ - rbc_code

OP_DE_WRITE:
	emit_code wde_code 
wde_code: 
	assume adl=0
	call write_de 
	assume adl=1 
wde_code.len := $ - wde_code 

OP_DE_READ: 
	emit_code rde_code
rde_code: 
	assume adl=0
	call read_de 
	assume adl=1 
rde_code.len := $ - rde_code



OP_ADDHLBC: 
	emit_code addhlbc_code
addhlbc_code: 
	assume adl=0 
	exx
	push bc 
	exx 
	pop hl 
	add hl,de 
	ex de,hl 
	assume adl=1 
addhlbc_code.len := $ - addhlbc_code


OP_ADDHLDE: 
	emit_code addhlde_code
addhlde_code: 
	assume adl=0 
	ex de,hl 
	add hl,bc  
	ex de,hl 
	assume adl=1 
addhlde_code.len := $ - addhlde_code

OP_ADDHLHL: 
	emit_code addhlhl_code
addhlhl_code: 
	assume adl=0 
	ex de,hl 
	add hl,hl 
	ex de,hl 
	assume adl=1 
addhlhl_code.len := $ - addhlhl_code

OP_ADDHLSP: 
	emit_code addhlsp_code
addhlsp_code: 
	assume adl=0 
	lea hl,iy+0 
	add hl,de 
	ex de,hl 
	assume adl=1 
addhlsp_code.len := $ - addhlsp_code


OP_EX_AF: 
	emit_code ex_af_code
ex_af_code: 
	assume adl=0 
	call exchange_af 
	assume adl=1 
ex_af_code.len := $ - ex_af_code

OP_EXX: 
	emit_code exx_code
exx_code: 
	assume adl=0 
	call exchange_af 
	assume adl=1 
exx_code.len := $ - exx_code



