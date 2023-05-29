
; special write routines 
section .text 
assume adl=1 

public mapper_write 
public stack_write_wram
public stack_write_sram

; HL = address
; a = page written to 
mapper_write:
	inc a 		; only care about writes to $FFFC+ 
	jr nz,.end  
	ld a,l 
	cp a,$FC
	call nc,swap_mapper
.end:
	pop af 
	pop.sis hl 
	jp.sis (hl) 
 

swap_mapper: 
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
	ld (slot2_sms),hl 
	exx 
	ret 

.rom: 
	; fetch rom page 
	exx 
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
	exx 
	ret		
.slot1_rom: ; FE 
	inc a 
	jr nz,.slot0_rom 
	ld de,$4000 
	or a,a 
	sbc hl,de 
	ld (slot1_smc),hl 
	exx 
	ret
.slot0_rom: ; FD
	ld (slot0_smc),hl 
	exx 
	ret 
	

	; de' = offset
	; (sps) = word to write 
stack_write_wram:  
	ld hl,_wram
	add hl,de 
	ld (hl),c 
	inc hl 
	ld (hl),b
	exx 
	ex af,af'
	pop.sis hl 
	jp.sis (hl) 
	
stack_write_sram:
	ld hl,_sram
	add hl,de 
	pop.sis de 
	ld (hl),c 
	inc hl 
	ld (hl),b
	exx
	ex af,af'
	pop.sis hl 
	jp.sis (hl)
	
	
extern slot0_smc
extern slot1_smc
extern slot2_smc

extern _sram 
extern _wram

extern _rom_page_lut 

