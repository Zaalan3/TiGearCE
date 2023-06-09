
section .text 

public _sram 
public _wram 
public _vram 
public _mapper_backup

public _portA
public _portB
public _port_GG_control

_vram := $D08000 
_sram := $D0C000 
_wram := $D14000
_mapper_backup := $D16000	; writes to $FFFC+ are copied here

include "z80space.inc" 
include "write.inc" 
include "opcode.inc" 
include "emit.inc" 


