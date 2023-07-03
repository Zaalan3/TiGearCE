
; special write routines 

assume adl=1 

; HL = address
; a = page written to 
mapper_write:
	ld a,l 
	cp a,$FC
	call nc,swap_mapper
.end: 
	pop.sis hl 
	jp.sis (hl) 
 

swap_mapper: 
	push de 
	ld a,(hl)	; local copy of mapper registers
	inc h 		; HL = $D160xx 
	ld (hl),a 
	jr nz,.rom 
	
.slot2_ram: 
	bit 4,a 	
	jr nz,.ram_enabled
.ram_disabled: 
	; fetch local copy of $FFFF 
	ld l,$FF 
	ld a,(hl) 
	jr .rom 
.ram_enabled:
	exx 
	ld hl,_sram 
	bit 2,a 
	jr nz,.end		; ram page select 
	set 6,h			; +16Kb 
.end: 	
	ld (slot2_smc),hl 
	exx 
	pop de
	ret 

.rom: 
	; fetch rom page 
	exx 
	push de 
	ld l,a 
	ld h,3 
	mlt hl 
	ld de,_rom_page_lut
	add hl,de 
	ld hl,(hl) 
	
	exx 
	ld a,l 
	exx 
	; select routine to write address to 
	sub a,$FF 
	jr nz,.slot1_rom 
.slot2_rom: ; FF
	exx 
	ld l,$FC ; skip if SRAM enabled 
	bit 4,(hl) 
	exx 
	jr nz,.slot2_skip
	ld de,$8000 
	or a,a 
	sbc hl,de 
	ld (slot2_smc),hl 
.slot2_skip: 
	pop de
	exx 
	ret		
.slot1_rom: ; FE 
	inc a 
	jr nz,.slot0_rom 
	ld de,$4000 
	or a,a 
	sbc hl,de 
	ld (slot1_smc),hl
	pop de
	exx 
	ret
.slot0_rom: ; FD
	ld (slot0_smc),hl 
	pop de
	exx 
	ret 
	
