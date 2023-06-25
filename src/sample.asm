
section .text

public _sample_code 

assume adl=0 
_sample_code: 
	ld a,5 
	ld (hl),a 
	bit 3,(hl) 
	ld b,c 
	ld c,h
	bit 1,l 
	jr nz,$+3 
	ld h,4 
	ld l,2 
	ld hl,5 
	and a,d 
	inc hl
	ret 
	
assume adl=1

