
public read_port_c 
public write_port_c 

public _portA
public _portB
public _portZero

read_port_c: 
	ld h,a 
	ld a,ixl 
	cp a,$08 
	jr c,io_gg_control_read
	cp a,$40 
	jq c,io_control_read 
	cp a,$80 
	jq c,vdp_counter
	cp a,$C0
	jq c,vdp_status  

controller_read:
	rra			; odd reads go to Port B 
	jr c,.portB 
.portA: 
	ld l,0 
_portA:=$-1
	rl l 
	rr l 
	ld a,h 
	ret 
.portB: 
	ld l,0
_portB:=$-1
	rl l 
	rr l 
	ld a,h 
	ret 
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
	rl l 
	rr l 
	ld a,h 
	ret 
	
; $00 - $3F range does nothing here, we ain't got no BIOS. 
io_control_read:
	ld l,$FF
	rl l 
	rr l 
	ld a,h 
	ret 

write_port_c: 
	push af 
	ld a,ixl 
	cp a,$08 
	jr c,io_gg_control_write
	cp a,$40 
	jr c,.end
	cp a,$80 
	jr c,.end 
	cp a,$C0 
	jq c,vdp_write
.end:
	pop af
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
_portZero := io_gg_registers 


