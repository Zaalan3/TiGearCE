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
 

extern uint8_t port_GG_control[7];

void* rom_page_lut[256];  

int main(void)
{
	port_GG_control[0] = 0;
    return 0;
}
