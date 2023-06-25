/*
 *--------------------------------------
 * Program Name:
 * Author:
 * License:
 * Description:
 *--------------------------------------
*/

#include <stdlib.h> 
#include <stdint.h>
#include <stdbool.h>
#include <string.h> 

#include <tice.h> 

extern uint8_t port_GG_control[7];

extern void jit_translate(uint8_t* dest,uint8_t* src); 

extern uint8_t sample_code[]; 

void* rom_page_lut[256]; 

int main(void)
{
	jit_translate((uint8_t*)0xD38000,sample_code); 
	
	while(!os_GetCSC()){}
	
    return 0;
}
