
public slot0_smc
public slot1_smc
public slot2_smc

	; memory page => handler routine  
	align $100
mapper_lut: 
	db 4  dup (fixed_routine and $FF)
	db 60 dup (slot0_routine and $FF)
	db 64 dup (slot1_routine and $FF)
	db 64 dup (slot2_routine and $FF) 
	db 32 dup (wram_routine and $FF)
	db 32 dup (wram_mirror_routine and $FF) 
	
	align $200 
mapper_routine:

; 0x0000 - 0x03FF always fixed to first rom page 
fixed_routine: 
	ld.lil hl,(_rom_page_lut) 
	add.lil hl,de 
	ret 
	
wram_routine: 
	ld.lil hl,_wram-$C000 
	add.lil hl,de 
	ret 
	
wram_mirror_routine: 
	ld.lil hl,_wram-$E000 
	add.lil hl,de 
	ret	
	
	
; example address function
slot0_routine:
	ld.lil hl,0 
slot0_smc_addr:= $-3 
	add.lil hl,de 	; add offset 
	ret 
	
slot1_routine: 
	ld.lil hl,0 
slot1_sms_addr:= $-3 
	add.lul hl,de 
	ret 
	
slot2_routine: 
	ld.lil hl,0 
slot2_sms_addr:= $-3 
	add.lil hl,de 
	ret 

slot0_smc := jit_page + slot0_smc_addr
slot1_smc := jit_page + slot1_smc_addr
slot2_smc := jit_page + slot2_smc_addr
	