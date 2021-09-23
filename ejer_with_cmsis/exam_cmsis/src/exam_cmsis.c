/*
===============================================================================
 Name        : exam_cmsis.c
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

#include "lpc17xx_timer.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

void confGpio(void);
void confTimer0(void);


int main(void) {
	//SystemInit();
	confGpio();
	confTimer0();
	while(1) {

    }
    return 0 ;
}


void confGpio(void){
	PINSEL_CFG_Type confPinsel;
	confPinsel.Portnum 			= 0; 	//Gpio Port 0
	confPinsel.Funcnum 			= 3; 	//MAT2.3
	confPinsel.Pinnum 		    = 9; 	//P0.9
	PINSEL_ConfigPin(&confPinsel); 		//configuro el P0.9 como Mat2.3
	return;
}
void confTimer0(void){
	TIM_TIMERCFG_Type conf_prescale;	//prescale timer
	TIM_MATCHCFG_Type conf_match; 		//match config
	//Configuracion prescaler
	conf_prescale.PrescaleOption 	= 0;	 				//No habilito la opcion de micro segundos, ya que lo hacer por defecto con clk/4 y yo quiero por 1.
	conf_prescale.PrescaleValue 	= 10000; 				//10us
	//Configuracion match
	conf_match.ExtMatchOutputType 	= 3; 					//toggle en el P0.9
	conf_match.IntOnMatch 		  	= 0;					//sin interrupcion
	conf_match.MatchChannel 	  	= 3;					//MAT2.3
	conf_match.StopOnMatch			= 0;
	conf_match.ResetOnMatch			= 1;					//reset en match
	conf_match.MatchValue 			= 99;					//Valor a cargar para establecer 10ms

	//inicializo el timer
	TIM_Init(LPC_TIM2, TIM_TIMER_MODE ,&conf_prescale);
	//Cambio el divisor de clock de perifericos al timer0 con divisor por 1(defecto viene por 4 = 25Mhz)
	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER2,CLKPWR_PCLKSEL_CCLK_DIV_1); //100Mhz
	//Configuro el match
	TIM_ConfigMatch(LPC_TIM2,&conf_match);
	//Habilito el timer
	TIM_Cmd(LPC_TIM2, 1);
	//Reinicio el timer para sincronizar
	TIM_ResetCounter(LPC_TIM2);
	return;
}
