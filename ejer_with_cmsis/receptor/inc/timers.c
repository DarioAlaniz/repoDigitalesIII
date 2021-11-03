/*
 * timers.c
 *
 *  Created on: 31 oct. 2021
 *      Author: PC
 */

#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "timers.h"



void confTimer1 (void) {
	TIM_TIMERCFG_Type timer1_conf;

	timer1_conf.PrescaleOption = TIM_PRESCALE_USVAL;
	timer1_conf.PrescaleValue  = PRESCALER_VALUE_TM1; 		//resolucion de 1us (si PCLK= 25MHz)

	TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &timer1_conf); 	//TIM_Init no le da enable
}

void confTimer2 (void) {
	TIM_TIMERCFG_Type timer2_conf;

	timer2_conf.PrescaleOption = TIM_PRESCALE_USVAL;
	timer2_conf.PrescaleValue  = PRESCALER_VALUE_TM2; 	//resolucion de 1ms (si PCLK= 25MHz)

	TIM_Init(LPC_TIM2, TIM_TIMER_MODE, &timer2_conf); 	//TIM_Init no le da enable
}

void delay_us (uint16_t time) {
	TIM_ResetCounter(LPC_TIM1);
	TIM_Cmd(LPC_TIM1, ENABLE);
	while (LPC_TIM1 -> TC < time); //en [us]
	TIM_Cmd(LPC_TIM1, DISABLE);
}

void delay_ms(uint16_t time){
	TIM_ResetCounter(LPC_TIM2);
	TIM_Cmd(LPC_TIM2, ENABLE);
	while (LPC_TIM2 -> TC < time); //en [us]
	TIM_Cmd(LPC_TIM2, DISABLE);
}

