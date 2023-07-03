
section .text

public _sample_code 

assume adl=0 
_sample_code: 
	xor a,a
    ld b,16
.l1: 
	add hl,hl
    rla
	ld a,(hl) 
    cp c
	rr (hl)
    jr c,.l2 
    sub c
    inc l
.l2:djnz .l1
	ret 
	
assume adl=1

