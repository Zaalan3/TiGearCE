


macro align n 
	rb n - $ 
end macro 

; Z80 page routines 
virtual at $0000 
	assume adl=0 
	
	align 0  
rst_jit_search:
	jp.lil jit_search
	 
	align $08
rst_jit_local: 
	jp.lil jit_search_local 
	
	align $10
rst_mapper: 
	jp mapper_get_address
rst_mapper_write: 
	jp.lil nc,mapper_write 
	
	align $18 
rst_verify_write_hl:
	push af
	ld a,d
	inc a 
	jr z,rst_mapper_write
	pop af 
	ret 
	
	align $20   
rst_verify_write_ind: 
	push af  
	ld a,ixh
	inc a 
	jr z,rst_mapper_write
	pop af 
	ret 
	
	; usage: push rr / rst 
	align $28
rst_push:
	pop hl
	exx
	jp push_handler   
	
	; usage: rst / pop rr
	align $30
rst_pop:
	pop hl
	exx 
	jp pop_handler 

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
	jp.lil jit_int_handler 
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
	
assert $ < $1FFF 
load _z80_page_data: $-$$ from $$
jit_page_len := $-$$
end virtual 
	
	assume adl = 1
jit_page_src: db _z80_page_data 

	