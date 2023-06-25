
	
pop_handler:
	lea de,iy+0
	ex af,af'
	ld.lil hl,i 
	ld l,d  
	ld l,(hl) 
	inc h 
	ld.lil hl,(hl) 
	add.lil hl,de 
	ld.lil hl,(hl) 
	ex af,af'
	push hl 
	exx
	lea iy,iy+2
	jp (hl)
	
	; If I seriously need write verification here, I will eat a thorny bush. 
push_handler:
	pop de 
	push bc
	ld b,d
	ld c,e
	lea de,iy-2
	ex af,af'
	ld.lil hl,i 
	ld l,d  
	ld l,(hl) 
	inc h 
	ld.lil hl,(hl) 
	add.lil hl,de
	ld.lil (hl),c 
	inc.lil hl 
	ld.lil (hl),b
	ex af,af' 
	pop bc
	exx 
	lea iy,iy-2
	jp (hl)

exchange_sp_hl:
	exx
	lea de,iy+0 
	ex af,af'
	ld.lil hl,i 
	ld l,d  
	ld l,(hl) 
	inc h 
	ld.lil hl,(hl) 
	add.lil hl,de 
	ld.lil hl,(hl) 
	ex af,af'
	push.lil hl 
	ld.lil hl,(hl) 
	push hl 
	exx 
	pop.lil hl 
	ld.lil (hl),de 
	pop de 
	ret 
