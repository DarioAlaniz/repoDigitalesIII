/*
===============================================================================
 Name        : test_Gpio.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here
#include "gpio.h"
// TODO: insert other definitions and declarations here
int main(void) {
	SystemInit();
	setConfGpio0(LPC_GPIO0, 22, 2, 1);
    while(1) {
    	setValueGpio(LPC_GPIO0, 22, 0);
    	for(int i=0;i<600000;i++);
    	setValueGpio(LPC_GPIO0, 22, 1);
    	for(int i=0;i<600000;i++);
    }
    return 0 ;
}
