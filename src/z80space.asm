
section .rodata 

jit_page := $D30000
jit_call_stack := $1FFE 

cycles_per_line := 228

macro align n 
	rb n - $ 
end macro 

public jit_page_src 
public jit_page_len
public jit_page
public jit_call_stack

public smc_int_enabled

; RST routines 
public rst_jit_dispatch
public rst_get_address_hl
public rst_get_address_ind
public rst_verify_write_hl
public rst_verify_write_ind
public rst_push 
public rst_pop
public rst_line

;mapper 
public fixed_area_smc
public slot0_smc
public slot1_smc
public slot2_smc

;stack 
public update_stack 

;io 
public read_port_c 
public write_port_c 

public _portA
public _portB
public _portZero

;vdp 




; Z80 page routines 
virtual at $0000 
	assume adl=0 
	
	align 0  
rst_jit_dispatch:
	jp.lil 0 ;jit_dispatch
	 
	; de = z80 hl
	align $08
rst_get_address_hl: 
	ex af,af' 
	ld l,d 
	ld h, (mapper_lut and $FF00) shr 8 
	ld l,(hl)  	; offset of mapper-routine
	inc h 
	jp (hl) 
	
	; iy = z80 ix/iy 
	; usage: exx / lea de,iy+n / rst
	align $10
rst_get_address_ind: 
	rst rst_get_address_hl 
	push hl 
	exx 
	pop hl 
	ret 
	
	align $18 
rst_verify_write_hl:
	push af 
	ld a,d 
	jp.lil mapper_write
	
	align $20   
rst_verify_write_ind: 
	push af  
	ld a,ixh
	jp.lil mapper_write
	
	; usage: push rr / rst 
	align $28
rst_push:
	exx
	jp push_handler   
rst_push_smc:=$-2 
	
	; usage: rst / pop rr
	align $30
rst_pop:
	ex af,af'
	exx 
	jp pop_handler 
rst_pop_smc:=$-2

	
	align $38
rst_line: 
	call vdp_line
	ld a,(vdp_flags) 
	and a,10000001b ; return if no pending interrupts 
	jr z,.end 
	ld a,0 ;service if interrupts enabled
smc_int_enabled:=$-1
	or a,a 
	jr z,.end 
	;jp.lil 0 ;jit_int_handler 
	;TODO: add tester for vdp register changes
.end: 
	ld a,cycles_per_line 
	ret 
	
	
	align $66 
nmi_handler: 
	ret 
	
include "mapper.inc" 
include "stack.inc" 
include "io.inc"
include "vdp.inc" 
	
load _z80_page_data: $-$$ from $$
jit_page_len := $-$$
end virtual 

jit_page_src: db _z80_page_data 

extern _sram 
extern _wram 
extern mapper_write 
;extern jit_dispatch
