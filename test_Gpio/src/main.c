/*
===============================================================================
 Name        : test_Gpio.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#define LED_RED 	22 	//gpio0 --> pinsel1[13:12]
#define LED_BLUE 	26	//gpio3 --> pinsel7[19:18]
#define LED_GREEN	25	//gpio3 --> pinsel7[21:20]

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here
#include "gpio.h"
#include "stdio.h"
// TODO: insert other definitions and declarations here



int main(void) {
	SystemInit();
//	setConfGpio(0, 22, 2, 1);
	LPC_PINCON->PINSEL7  &= 	~(0b11<<18);
	LPC_PINCON->PINMODE7 &= 	~(0b1<< 18);
	LPC_PINCON->PINMODE7 |=  	(0b1<< (18+1));
	LPC_GPIO3->FIODIR 	 |= 	(1<<LED_BLUE); //output

    while(1) {
    	setValueGpio(LPC_GPIO3, 26, 0);
    	for(int i=0;i<600000;i++);
    	setValueGpio(LPC_GPIO3, 26, 1);
    	for(int i=0;i<600000;i++);
    }
    return 0 ;
}
