
section .rodata 

jit_page := $D30000
jit_shadow_stack := $0F00  
jit_call_stack := $1FFE 

macro align n 
	rb n - $ 
end macro 


; Z80 page routines 
virtual at $0000 
	assume adl=0 
	
	align 0  
rst_trap: 
	ret 
	 
	; de = z80 hl
	align $08
rst_get_address_hl: 
	ex af,af' 
	ld l,d 
	ld h,mapper_lut>>8 
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
	ld a,iyh
	jp.lil mapper_write
	
	; usage: push rr / rst 
	align $28
rst_push: 
	ex af,af' 
	exx
	ld hl,i 
	dec hl 
	dec hl
	jp push_in_wram   
rst_push_smc:=$-2 
	
	; usage: rst / pop rr
	align $30
rst_pop: 
	ex af,af' 
	exx 
	ld hl,i 
	inc hl 
	inc hl 
	jp pop_in_wram 
rst_pop_smc:=$-2

	
	align $38
rst_int_poll: 
	ret 
	
	align $66 
nmi_handler: 
	ret 
	
	include "mapper.inc" 
	include "stack.inc" 
	include "io.inc"
	
load _z80_page_data: $-$$ from $$
jit_page_len := $-$$
end virtual 

jit_page_src: db _z80_page_data 

extern _sram 
extern _wram 
extern mapper_write 

