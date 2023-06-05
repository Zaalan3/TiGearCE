; public vdp_flags 
; public vdp_vcounter

; public vdp_cram_mode
; public vdp_active_height

; public vdp_cram 
; public vdp_cram_mask
; public vdp_cram_written_flag

; public vdp_registers 
; public vdp_registers_written_flag

; public vdp_tile_flags 


vdp_line:
	ld a,(vdp_flags)
	ld hl,(vdp_vcounter) 
	inc hl 
	ld (vdp_vcounter),hl 
	ex de,hl 
	jp .active_display
vdp_area_smc:=$-2 

.vblank:
	ld hl,.border_display
	ld (vdp_area_smc),hl 
	ld hl,(vdp_registers)	; fetch control registers  
	bit 5,h					; trigger if IE0 high
	ret z 
	set 7,a 
	ld (vdp_flags),a 
	; jp.lil vdp_draw
	ret
	
.active_display:
	ld a,192 	
vdp_active_height:=$-1
	cp a,e 
	jr z,.vblank
	
	ld b,0 
vdp_line_counter:=$-1 
	dec b 
	jp po,.line_interrupt
	ld a,b 
	ld (vdp_line_counter),a 
	ret
	
.line_interrupt:
	ld hl,(vdp_registers)	; fetch control registers  
	bit 4,l					; trigger if line interrupts enabled 
	jr z,.line_reload 
	set 0,a 
	ld (vdp_flags),a
	ret 
.line_reload:
	ld a,(vdp_registers+$0A) 	; line counter reload value
	ld (vdp_line_counter),a
	ret 
	
.border_display:
	ld hl,262 
	or a,a 
	sbc hl,de
	ret nz
	ld (vdp_vcounter),hl
	ld hl,.active_display
	ld (vdp_area_smc),hl
	jr .line_reload
	

	

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
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
	ld hl,i
	ex de,hl
	ld.lil hl,_vram 	
	add.lil hl,de
	ld.lil a,(hl)
	ld (vdp_read_buffer),a 
	ex de,hl
	inc hl
	res 6,h			; mask to 14 bits 
	ld i,hl
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
	
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
; l = byte to write
vdp_write: 
	rra 
	jq c,vdp_write_control
	
vdp_write_data: 
	ccf
	ld a,l 
	exx
	jr c,.cram
.vram: 
	ld hl,i
	ex de,hl
	ld.lil de,_vram
	add.lil hl,de
	ld.lil (hl),a
	ex de,hl
	inc hl 
	res 6,h 
	ld (vdp_vram_address),hl 
	
	; tile_flags[address>>8] = true (for tile cache updates)
	ld d,vdp_tile_flags shr 8 
	ld e,h 
	set 7,e
	ld a,d		; any nonzero value will do
	ld (de),a 
	pop af 
	exx 
	ret 
	
.cram:
	jp .cramGG
vdp_cram_mode:=$-2

.cramSMS:
	ld a,i
	and a,00011111b
	ld d,vdp_cram shr 8
	ld e,a
	ex de,hl
	ld (hl),e 
	ld l,vdp_cram_written_flag and $FF
	ld (hl),l
	ld hl,i 
	inc hl
	res 6,h
	ld i,hl 
	pop af 
	exx 
	ret 

.cramGG: 
	ld a,i
	and a,00111111b
	ld d,vdp_cram shr 8
	ld e,a
	ex de,hl
	ld (hl),e
	bit 0,e
	jr z,.skipUpdateFlag
	ld l,vdp_cram_written_flag and $FF
	ld (hl),l
.skipUpdateFlag:
	ld hl,i 
	inc hl
	res 6,h
	ld i,hl 
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
	res 7,h	; 14-bit address 
	res 6,h
	ld i,hl
	
	rlca 
	rlca 
	and a,0011b
	jr z,.vram_read
	dec a 
	jr z,.set_data_vram 	; vram_write
	dec a 
	jr z,.register_write 
	
.cram_write:
	; set writes to data ports to go to cram
	ld a,$37 		; scf 
	ld (vdp_write_data),a
	jr .end 
	
.vram_read: 
	ex de,hl
	ld.lil hl,_vram
	add.lil hl,de
	ld.lil a,(hl) 				; reload read buffer
	ex de,hl
	inc hl 
	res 6,h
	ld i,hl				
	ld (vdp_read_buffer),a		; reload read buffer
	jr .set_data_vram 

.register_write:
	ld a,h 
	and a,$0F 
	ld h,a 
	ld a,l
	ld l,h
	ld de,vdp_registers
	ld h,0  
	add hl,de 
	ld (hl),a
	; set flag if any register <= 6 written to 
	ld a,l 
	and a,7 
	cp a,7 
	jr nc,.set_data_vram
	ld l,vdp_registers_written_flag and $FF 
	ld (hl),l

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
	
align $680 
vdp_tile_flags: 
	db 0 dup 64  

extern _vram 
