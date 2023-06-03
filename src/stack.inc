

pop_handler: 
	lea de,iy+0
	push bc 
	rst rst_get_address_hl
	ld.lil c,(hl)
	inc de
	rst rst_get_address_hl
	ld.lil b,(hl)
	pop de 
	push bc 
	ld b,d 
	ld c,e 
	exx
	pop hl
	lea iy,iy+2
	ret 
	
	; If I seriously need write verification here, I will eat a thorny bush. 
push_handler:
	pop hl 
	pop de 
	push hl 
	push bc
	ld b,d
	ld c,e
	ld de,iy-2
	rst rst_get_address_hl
	ld.lil (hl),c 
	inc de
	rst rst_get_address_hl
	ld.lil (hl),b
	exx 
	lea iy,iy-2
	ret 


