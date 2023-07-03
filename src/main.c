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

extern void jit_translate(uint8_t* src); 

extern uint8_t sample_code[]; 

int main(void)
{
	jit_translate((uint8_t*)0xD38000,sample_code); 
	
	while(!os_GetCSC()){}
	
    return 0;
}
