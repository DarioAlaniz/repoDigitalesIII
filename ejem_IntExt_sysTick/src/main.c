/*
===============================================================================
 Name        : ejem_IntExt_sysTick.c
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
#define PIN_OUT			9
#define PIN_EINT		10
#define LED_BLUE 		26	//gpio3 --> pinsel7[19:18]

void configGpio(void);
void configIntExt(void);
void configSysTick(void);

uint8_t period = 1;
uint8_t flag_Eint0 = 0;
int main(void) {
	configGpio();
	configIntExt();
	configSysTick();

	while(1){
		if (flag_Eint0) {
			LPC_GPIO3->FIOPIN ^= (1<<LED_BLUE);
			flag_Eint0 = 0;
		}
	}

}

void configGpio(void){
	LPC_PINCON->PINSEL0	 	&= ~(3<<(PIN_OUT*2)); 			//P0.09 gpio
	//pull up por defecto
	LPC_GPIO0->FIODIR		|= 	(1<<PIN_OUT);				//P0.09 output

	//LED
	LPC_PINCON->PINSEL7 	&= ~(3<<18);				//P0.26
	LPC_GPIO3->FIODIR 		|= (1<<LED_BLUE);

	return ;
}

void configIntExt(void){
	LPC_PINCON->PINSEL4 |= 	(1<<(PIN_EINT*2)); 			//P2.10 EINT0
	//pull up por defecto
	LPC_SC->EXTMODE 	|= 1;			// interrupcion por flanco
	LPC_SC->EXTPOLAR	&= ~1;			// flanco de bajada

	LPC_SC->EXTINT		|= 1;			//limpio el flag de EINT0, siempre se debe
										//hacer por cada cambio en los registros anterior
										//antes de habilitar las interrupciones

	NVIC_SetPriority(EINT0_IRQn, 5);	//establesco prioridad 5
	NVIC_EnableIRQ(EINT0_IRQn);			//habilito la interrupcion por EINT0

	return ;
}

void configSysTick(void){
	SysTick->CTRL 	|= (1<<0) | (1<<1) | (1<<2);					//	habilito el contador,
	SysTick->LOAD  	|= ((SystemCoreClock / 1000) * period) - 1; 	// 	arranco con periodo de 1ms, max 167,7 ms
	SysTick->VAL  	=0;												//	arranca en cero
	NVIC_SetPriority(SysTick_IRQn, 4);								// 	prioridad 4 para que sea mas importante que el EINT0
																	// 	asi mientras espera que pase el tiempo para el anti rebote pueda interruptir
	//SysTick->CTRL;												// 	leo el registro para borrar el counter flag
	return ;
}

void EINT0_IRQHandler(void){
	flag_Eint0 = 1;
	period+=1;
	if(period>167) period=1;
	for (int i = 0; i<1000000;i++); 		//retardo antes de bajar el flag asi evito efecto rebote
	LPC_SC->EXTINT |= 1;				//limpio el flag de interrupcion
}


void SysTick_Handler(void){
	LPC_GPIO0->FIOPIN  	^= (1<<PIN_OUT);
	SysTick->LOAD  	     = ((SystemCoreClock / 1000) * period) - 1;
	SysTick->VAL  		 =0;
	SysTick->CTRL;
	// no hace falta limpiar la bandera, pero se limpia leyendo el registro STCTRL
}
