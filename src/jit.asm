
section .text 

public _sram 
public _wram 
public _vram 
public _mapper_backup

public _portA
public _portB
public _port_GG_control

_jitEntry := $D04000
_vram := $D08000 
_sram := $D0C000 
_wram := $D14000
_mapper_backup := $D16000	; writes to $FFFC+ are copied here

jit_page := $D30000
jit_call_stack := $1FFE 
cycles_per_line := 228

include "z80space.inc" 
include "write.inc" 
include "opcode.inc" 

public _jit_translate 
public jit_init 

jit_init: 
	ret 
jit_search: 
	ret 
jit_search_local: 
	ret 
	
jit_int_handler: 
	ret 
jit_add_block: 
	ret 
jit_flush:
	ret 
	


section .bss

; void* arr[256] 
_rom_page_lut: 
	rb 768 
	
; block entry:  
; dl z80 addr 
; dw cache addr 
_jit_block: 
	rb 5*3*1024 
	
; block list 
jit_next_entry: 
	rb 3
jit_next_addr: 
	rb 3 

; block 
block_start_addr: 
	rb 3 
block_cycles_addr: 
	rb 3  
block_next_split:
	rb 3

block_z80_addr: 
	rb 3
block_page_low: 
	rb 3 
block_page_high: 
	rb 3 

block_cache_addr: 
	rb 3


public _rom_page_lut

public jit_next_entry
public jit_next_addr
	
public block_start_addr
public block_cycles_addr
public block_next_split

public block_z80_addr
public block_page_low
public block_page_high

public block_cache_addr
