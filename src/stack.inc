
extern stack_write_wram
extern stack_write_sram
extern _mapper_backup

; perform two byte reads if in region boundary / mapper region 
pop_region_boundary: 
	ld hl,i 
	ex de,hl 
	ex af,af' 
	rst rst_get_address_hl
	ld.lil c,(hl) 
	inc de
	rst rst_get_address_hl
	ld.lil b,(hl)
	call push_region_boundary.update
	pop hl 
	push bc 
	push hl 
	exx
	ret 
	
; perform two byte writes if in region boundary / mapper region 
push_region_boundary:
	exx 
	push bc 
	push de 
	exx 
	push bc 
	exx 
	pop bc 
	ld hl,i 
	ex de,hl 
	ex af,af' 
	rst rst_get_address_hl
	ld.lil (hl),c 
	rst rst_verify_write_hl
	inc de
	rst rst_get_address_hl
	ld.lil (hl),b
	rst rst_verify_write_hl
	pop de 
	pop bc 
.update:
	ex af,af' 
	; update rst jp's 
	ld h,0 
	ld l,d 
	add hl,hl
	add hl,hl
	add hl,hl
	add hl,hl
	ld de,stack_routine_lut
	add hl,de 
	ld de,(hl) 
	inc hl 
	inc hl 
	ld hl,(hl) 
	ld (rst_pop_smc),hl 
	ex de,hl 
	ld (rst_push_smc),hl 
	ex af,af'
	ret 

stack_routine_lut: 
	dw 8 dup (push_in_rom,pop_in_rom)
	dw 4 dup (push_in_slot2,pop_in_rom) 
	dw 2 dup (push_in_wram,pop_in_wram) 
	dw 2 dup (push_in_wram_mirror,pop_in_wram_mirror) 
	
	
push_in_rom:
	pop de 
	pop bc 
	push de 
	ld i,hl 
	inc h 
	jq z,push_region_boundary 
	ret 

push_in_wram:
	pop de 
	pop bc 
	push de
	ld i,hl
	ld de,$C000 
	or a,a 
	sbc hl,de 
	jq c,push_region_boundary
	ex de,hl
	jp.lil stack_write_wram

push_in_wram_mirror:
	pop de 
	pop bc 
	push de
	ld i,hl
	ld de,$E000 
	or a,a 
	sbc hl,de 
	jq c,push_region_boundary
	ex de,hl
	ld hl,i 
	ld bc,$FFFC 
	or a,a 
	sbc hl,bc 
	jq nc,push_region_boundary
	jp stack_write_wram
	
	
push_in_slot2:
	pop de 
	pop bc 
	push de
	ld i,hl
	ld de,$8000 
	or a,a 
	sbc hl,de 
	jq c,push_region_boundary
	ex de,hl 
	ld.lil hl,(_mapper_backup + $FC) 
	bit 4,l 	; test if SRAM is enabled
	jr nz,.sram_enabled 
	ex af,af' 
	exx 
	ret 
.sram_enabled:
	bit 2,l 	; SRAM page select 
	jr nz,.skip 
	set 6,d 	; +16KB
.skip: 
	jp.lil stack_write_sram 
	
	
pop_in_rom: 
	ld i,hl
	ex de,hl
	inc e ; only read twice if on a page boundary  
	dec e
	jp pe,.readtwice
.readonce: 
	dec e 
	ex af,af' 
	rst rst_get_address_hl
	ld.lil bc,(hl)
	jr .end 
.readtwice:
	dec e 
	ex de,hl 
	ex af,af' 
	rst rst_get_address_hl
	ld.lil c,(hl) 
	inc de
	rst rst_get_address_hl
	ld.lil b,(hl)
.readtwice:
	pop hl 
	push bc 
	push hl 
	exx
	ret 
	
pop_in_wram: 
	ld i,hl
	ld de,$E000 
	or a,a 
	sbc hl,de 
	jq nc,pop_region_boundary
	ld de,$2000 	; adjust to hl-$C000 
	add hl,de
	ld.lil de,_wram 
	add.lil hl,de 
	ld.lil hl,(hl)
	pop de 
	push hl
	push de 
	exx 
	ex af,af'
	ret
	
pop_in_wram_mirror:
	ld i,hl 
	dec h 
	jp pe,pop_region_boundary
	inc h 
	ld de,$E000 
	or a,a 
	sbc hl,de 
	ld.lil de,_wram 
	add.lil hl,de 
	ld.lil hl,(hl)
	pop de 
	push hl
	push de 
	exx 
	ex af,af'
	ret