
exchange_af: 
	push af 
	pop hl 
	ld sp,exchange_stack_af 
	ex (sp),hl 
	ld sp,jit_call_stack 
	push hl 
	pop af 
	ret 
	
exchange: 
	ld sp,exchange_stack 
	ex de,hl 
	ex (sp),hl 
	ex de,hl 
	pop hl 
	ld h,b 
	ld l,c 
	ex (sp),hl  
	ld b,h 
	ld c,l 
	pop hl 
	exx 
	ld h,b 
	ld l,c 
	ex (sp),hl  
	ld b,h 
	ld c,l 
	exx 
	ld sp,jit_call_stack 
	ret 
	
exchange_stack_af: 
	dw 0,0,0 
exchange_stack: 
	dw 0 
	
read_de: 
	ex af,af'
	ld.lil hl,i 
	ld l,b  
	ld l,(hl) 
	inc h 
	ld.lil hl,(hl) 
	add.lil hl,bc 
	ex af,af' 
	ld.lil a,(hl)
	ret 
	
write_de: 
	ex af,af'
	ld.lil hl,i 
	ld l,b  
	ld l,(hl) 
	inc h 
	ld.lil hl,(hl) 
	add.lil hl,bc 
	ex af,af' 
	ld.lil (hl),a 
	push af 
	ld a,b 
	inc a 
	jr nz,$+3  
	rst rst_mapper_jmp
	pop af 
	ret 
	
	
	