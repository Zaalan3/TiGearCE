
vdp_line: 

vdp_counter_read: 
	rra 
	jr c,.hcounter
	ld a,(vdp_vcounter)
	ld l,a
	rl l 
	rr l 
	ld a,h 
	ret 
.hcounter: 
	; returns a fixed value at right blanking 
	ld l,$88
	rl l 
	rr l 
	ld a,h 
	ret 

vdp_read:
	rra
	jr c,.status 
.vram:
	ld l,0 
vdp_read_buffer:=$-1
	exx 
	ld de,(vdp_read_address) 
	ld.lil hl,_vram 
	add.lil hl,de 
	ld.lil a,(hl) 
	ld (vdp_read_buffer),a 
	ex de,hl 
	inc hl 
	ld a,h 
	and a,00111111b ; mask to 14 bits 
	ld h,a 
	ld (vdp_read_address),hl  
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
	
vdp_write: 
	rra 
	jr c,.control 
.data: 
	
.control: 
	ccf 
	jr c,.secondbyte 
	ld a,l 
	ld (vdp_command_word_low),a
	ld a,$37	; scf 
	ld (vdp_write.control),a 
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
	ld a,l 
	and a,0 	; 5 bits in SMS mode, 6 bits in GG mode
vdp_cram_mask := $-1
	ld h,$80 and (vdp_cram shr 8)
	ld l,a 
	ld (vdp_write_address),hl 
	jr .end 
.vram_read: 
	ld a,h 
	and a,00111111b 
	ld h,a 
	ex de,hl
	ld.lil hl,_vram 
	add.lil hl,de 
	ld.lil a,(hl) 
	ld (vdp_read_buffer),a
	jr .end 
.vram_write:
	ld a,h
	and a,00111111b 
	ld h,a 
	ld (vdp_write_address),hl 
	jr .end
.register_write: 
	ld a,h 
	ld c,a 
	and a,$0F 
	ld de,vdp_registers
	ld h,0 
	ld l,a 
	add hl,de 
	ld (hl),c 
.end: 
	exx 
	ld a,$3F	; ccf 
	ld (vdp_write.control),a 
	pop af
	ret 


vdp_write_address: dw 0 
vdp_read_address: dw 0 

vdp_flags: db 0
vdp_vcounter: db 0 

vdp_registers: 
	db 16 dup 0 
	
align $500 
vdp_cram:  
	db 0 dup 64

align $600 
vdp_tile_flags: 
	db 0 dup 512 

extern _vram 
