; public fixed_area_smc
; public slot0_smc
; public slot1_smc
; public slot2_smc

; memory page => handler routine  
align $100
mapper_lut: 
	db 4  dup (fixed_area_smc_addr and $FF)	; 0x0000 - 0x03FF always fixed to first rom page 
	db 60 dup (slot0_smc_addr and $FF)		; 400 - 3FFF 
	db 64 dup (slot1_smc_addr and $FF)		; 4000 - 7FFF 
	db 64 dup (slot2_smc_addr and $FF) 		; 8000 - BFFF 
	db 32 dup (wram_fetch and $FF)			; C000 - DFFF 
	db 32 dup (wram_mirror_fetch and $FF) 	; E000 - FFFF 
	
align $200 
mapper_routine:


fixed_area_smc_addr: 
	emit 3: 0 
	
slot0_smc_addr:
	emit 3: 0
	
slot1_smc_addr: 
	emit 3: 0
	
slot2_smc_addr: 
	emit 3: 0
	
wram_fetch: 
	emit 3: _wram - $C000 
	
wram_mirror_fetch:
	emit 3: _wram - $E000 
	


fixed_area_smc := jit_page + fixed_area_smc_addr
slot0_smc := jit_page + slot0_smc_addr
slot1_smc := jit_page + slot1_smc_addr
slot2_smc := jit_page + slot2_smc_addr
	