public fixed_area_smc
public slot0_smc
public slot1_smc
public slot2_smc

; memory page => handler routine  
align $100
mapper_lut: 
	db 4  dup (fixed_area_fetch and $FF)
	db 60 dup (slot0_fetch and $FF)
	db 64 dup (slot1_fetch and $FF)
	db 64 dup (slot2_fetch and $FF) 
	db 32 dup (wram_fetch and $FF)
	db 32 dup (wram_mirror_fetch and $FF) 
	
align $200 
mapper_routine:

; 0x0000 - 0x03FF always fixed to first rom page 
fixed_area_fetch: 
	ld.lil hl,0
fixed_area_smc_addr:=$-3
	add.lil hl,de 
	ret 
	
wram_fetch: 
	ld.lil hl,_wram-$C000 
	add.lil hl,de 
	ret 
	
wram_mirror_fetch: 
	ld.lil hl,_wram-$E000 
	add.lil hl,de 
	ret	
	
	
; example address function
slot0_fetch:
	ld.lil hl,0 
slot0_smc_addr:= $-3 
	add.lil hl,de 	; add offset 
	ret 
	
slot1_fetch: 
	ld.lil hl,0 
slot1_smc_addr:= $-3 
	add.lil hl,de 
	ret 
	
slot2_fetch: 
	ld.lil hl,0 
slot2_smc_addr:= $-3 
	add.lil hl,de 
	ret 

fixed_area_smc := jit_page + fixed_area_smc_addr
slot0_smc := jit_page + slot0_smc_addr
slot1_smc := jit_page + slot1_smc_addr
slot2_smc := jit_page + slot2_smc_addr
	