
jit_page := $D30000
jit_call_stack := $1FFE 

cycles_per_line := 228

macro align n 
	rb n - $ 
end macro 

; Z80 page routines 
virtual at $0000 
	assume adl=0 
	
	align 0  
rst_jit_dispatch:
	jp.lil 0 ;jit_dispatch
	 
	align $08
rst_jit_local: 
	
	align $10
rst_hex10: 
	
	align $18 
rst_verify_write:
	inc a
	jp.lil z,mapper_write
	ret 
	
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
	exx 
	jp pop_handler 
rst_pop_smc:=$-2

	
	align $38
rst_line:
	exx
	call vdp_line
	exx
	ld a,(vdp_flags) 
	and a,10000001b ; return if no pending interrupts 
	jr z,.end 
	ld a,0 ;service if interrupts enabled
z80_int_enabled:=$-1
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
include "ophandle.inc"
	
load _z80_page_data: $-$$ from $$
jit_page_len := $-$$
end virtual 
	
	assume adl = 1
jit_page_src: db _z80_page_data 

	