public vdp_flags 
public vdp_vcounter

public vdp_cram 
public vdp_cram_mask
public vdp_cram_written_flag

public vdp_registers 
public vdp_registers_written_flag

public vdp_tile_flags 


vdp_line:
	exx
	ld a,(vdp_flags)
	ld bc,(vdp_registers)	; fetch control registers 
	; test for vertical blank 
	ld hl,(vdp_vcounter) 
	inc hl 
	ld (vdp_vcounter),hl 
	ex de,hl 
	
	ld hl,262 
	or a,a 
	sbc hl,de
	jr z,.line0
	
	ld hl,192
	bit 1,c		; test if in 192 or 224 line mode (M2 & M1)
	jr z,.test
	bit 4,b 
	jr z,.test
	ld hl,224 
	
.test: 
	or a,a 
	sbc hl,de 
	jr nc,.active_display
.vblank: 
	jr nz,.end 
	bit 5,b	; trigger if IE0 high
	jr z,.end
	set 7,a 
	ld (vdp_flags),a 
	; jp.lil vdp_draw
	exx 
	ret
.line0: 
	ex de,hl 
	ld (vdp_vcounter),hl
	jr .line_reload
.active_display: 
	ld b,0 
vdp_line_counter:=$-1 
	dec b 
	jp po,.line_interrupt
	ld a,b 
	ld (vdp_line_counter),a 
	exx 
	ret 
.line_interrupt: 
	bit 4,c		; trigger if line interrupts enabled 
	jr z,.line_reload 
	set 0,a 
	ld (vdp_flags),a
.line_reload: 
	ld a,(vdp_registers+$0A) 	; line counter reload value
	ld (vdp_line_counter),a
.end: 
	exx 
	ret 	
	
vdp_counter_read: 
	rra 
	jr c,.hcounter
	exx 
	ld de,0
vdp_vcounter:=$-2 
	ld a,e 
	exx 
	ld l,a 
	ld a,h 
	ex af,af' 
	rl l 
	rr l 
	ret 
.hcounter: 
	; returns a fixed value at right blanking 
	ld l,$88
	rl l 
	rr l 
	ld a,h 
	ret 

; l = byte read
vdp_read:
	rra
	jr c,.status 
.vram:
	ld l,0 
vdp_read_buffer:=$-1
	exx 
	ld de,(vdp_vram_address) 
	ld.lil hl,_vram 	; hlu static 
	ld h,d 
	ld l,e 
	ld.lil a,(hl) 
	ld (vdp_read_buffer),a  
	inc hl 
	res 6,h			; mask to 14 bits 
	ld (vdp_vram_address),hl
	exx 
	rl l 
	rr l 
	ld a,h 
	ret 
	
.status: 
	ld hl,vdp_flags
	ld a,(hl) 
	ld (hl),0 	; clear flags after read
	ld l,a 
	rl l 
	rr l 
	ld a,h 
	ret 
	
	
; l = byte to write
vdp_write: 
	rra 
	jr c,vdp_write_control
	
vdp_write_data: 
	ccf
	ld a,l 
	exx
	jr c,.cram
.vram: 
	ld de,(vdp_vram_address) 
	ld.lil hl,_vram
	ld h,d 
	ld l,e 
	ld.lil (hl),a
	inc hl 
	res 6,h 
	ld (vdp_vram_address),hl 
	; tile_flags[address>>5] = true (for tile cache updates)
	
	ld d,vdp_tile_flags shr 8 
	add hl,hl
	add hl,hl 
	add hl,hl 
	jr nc,.skip 
	inc d 	; second page of tiles 
.skip: 
	ld e,h 
	ld a,d
	ld (de),a 
	pop af 
	exx 
	ret 
	
.cram:
	ld a,l
	ld l,0 
vdp_cram_address:=$-1
	ld h,vdp_cram shr 8 
	ld (hl),a 
	ld a,l 
	inc l 
	ld a,00011111b 
vdp_cram_mask:=$-1 
	and a,l 
	ld (vdp_cram_address),a 
	ld l,vdp_cram_written_flag and $FF 
	ld (hl),l
	pop af 
	exx 
	ret 

vdp_write_control: 
	ccf 
	jr c,.secondbyte 
	ld a,l 
	ld (vdp_command_word_low),a
	ld a,$37	; scf 
	ld (vdp_write_control),a 
	pop af 
	ret 
.secondbyte: 
	ld a,l 
	exx 
	ld h,a 
	ld l,0
vdp_command_word_low:=$-1 

	; get command code(a>>6) 
	rlca 
	rlca 
	or a,0011b 
	jr z,.vram_read
	dec a 
	jr z,.vram_write 
	dec a 
	jr z,.register_write 
	
.cram_write:
	; set writes to data ports to go to cram
	ld a,$37 		; ccf 
	ld (vdp_write_data),a
	ld a,(vdp_cram_mask) 
	and a,l 
	ld (vdp_cram_address),a
	jr .end 
	
.vram_read: 
	ld a,h
	and a,00111111b 
	set 7,a
	ld h,a
	ex de,hl
	ld.lil hl,_vram
	ld h,d 
	ld l,e
	ld.lil a,(hl) 				; reload read buffer
	inc hl
	res 6,h						
	ld (vdp_read_buffer),a		; reload read buffer
	ld (vdp_vram_address),hl	; save bottom 16 bits of vram address
	jr .set_data_vram 

.register_write:
	push hl 
	ld a,h 
	ld c,a 
	and a,$0F 
	ld de,vdp_registers
	ld h,0 
	ld l,a 
	add hl,de 
	ld (hl),c
	ld l,vdp_registers_written_flag and $FF 
	ld (hl),l
	pop hl
	
.vram_write:
	ld a,h
	and a,00111111b 
	set 7,a
	ld h,a 
	ld (vdp_vram_address),hl
.set_data_vram:
	ld a,$3F 		; ccf 
	ld (vdp_write_data),a
.end: 
	exx 
	ld a,$3F	; ccf 
	ld (vdp_write_control),a
	pop af
	ret 



vdp_vram_address: dw 0
vdp_flags: db 0

align $600 
vdp_cram:  
	db 0 dup 64

align $650 
vdp_cram_written_flag: 
	db 0

align $660 
vdp_registers: 
	db 16 dup 0	
align $670 
vdp_registers_written_flag: 
	db 0 
	
align $700 
vdp_tile_flags: 
	db 0 dup 512 

extern _vram 