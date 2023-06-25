; public read_port_c 
; public write_port_c 

; public _portA
; public _portB
; public _port_GG_control

; output: l = byte read 
read_port_c: 
	ld h,a 
	exx 
	ld a,c
	exx 
	cp a,$07
	jr c,io_gg_control_read
	cp a,$40 
	jq c,io_control_read 
	cp a,$80 
	jq c,vdp_counter_read
	cp a,$C0
	jq c,vdp_read 

controller_read:
	rra			; odd reads go to Port B 
	jr c,.portB 
.portA: 
	ld l,0 
portA:=$-1
	ld a,h 
	ret 
.portB: 
	ld l,0
portB:=$-1
	ld a,h 
	ret 

io_gg_control_read: 
	exx 
	ld h,0 
	ld l,a 
	ld de,io_gg_registers
	add hl,de 
	ld a,(hl) 
	exx 
	ld l,a 
	ld a,h 
	ret 
	
; $00 - $3F range does nothing here, we ain't got no BIOS. 
io_control_read:
	ld l,$FF
	ld a,h 
	ret 

;input: l = byte to write
write_port_c: 
	push af 
	exx 
	ld a,c 
	cp a,$08 
	jr c,io_gg_control_write
	cp a,$80 
	jr c,.end 
	cp a,$C0 
	jq c,vdp_write
.end:
	pop af
	exx
	ret 
	
io_gg_control_write: 
	or a,a 		; port 0 is read only
	jr z,.skip 
	ld a,l
	exx 
	ld h,0 
	ld l,a 
	ld de,io_gg_registers
	add hl,de 
	ld (hl),a
	exx 
.skip: 
	pop af 
	ret 
	
io_gg_registers: 
	db $C0,$7F,$FF,$00,$FF,$00,$FF

; C code needs access to set start button(bit 7)
_port_GG_control := jit_page + io_gg_registers 
_portA := jit_page + portA 
_portB := jit_page + portB 


