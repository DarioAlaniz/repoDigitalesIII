/*
===============================================================================
 Name        : ejer_adc.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : ejemplo simple de conversion y salida por el dac
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_gpio.h"

#define FREQ_SAMPLE		10000

void confPin(void);
void confAdc(void);
void confTimer0(void);
void confDac(void);


uint16_t adc_value;
int main(void) {
	confPin();
	confTimer0();
	confAdc();
	confDac();
	TIM_Cmd(LPC_TIM0, ENABLE);							//habilito el timer
	TIM_ResetCounter(LPC_TIM0);							//reset el timer
    while(1) {

    }
    return 0 ;
}
void confPin(void){
	PINSEL_CFG_Type confPin	= {0}; 	//inicilizo toda la struc en 0
	confPin.Portnum			= 0;
	confPin.Pinnum			= 23;  	//P0.23
	confPin.Pinmode			= 2; 	//ni pull up, ni pull dowm
	confPin.Funcnum			= 1;	//AD0.0 canal 0 del adc
	PINSEL_ConfigPin(&confPin);
	confPin.Pinnum			= 26;	//P0.26
	confPin.Funcnum			= 2;	//AOUT
	PINSEL_ConfigPin(&confPin);
	confPin.Pinnum			= 9;	//
	confPin.Pinmode			= 0; 	//pull up
	confPin.Funcnum			= 0;	//Gpio
	PINSEL_ConfigPin(&confPin);
	GPIO_SetDir(0,(1<<9),1);		//output
}

void confAdc(void){
	ADC_Init(LPC_ADC, FREQ_SAMPLE); 					//power on, habilito el PCADC
	ADC_PowerdownCmd(LPC_ADC, ENABLE);					//pongo en funcionamiento el ADC
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE); 	//habilito solo el canal 0
	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01);			//habilito el adc para cuando hace match con el timer0
	ADC_EdgeStartConfig(LPC_ADC, 1);
	ADC_IntConfig(LPC_ADC,ADC_ADGINTEN,RESET);
	ADC_IntConfig(LPC_ADC,ADC_ADINTEN0,ENABLE);			//habilito la interrupcion para el canal 0
	adc_value = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_0);  //limpio la badera de interrupcion
	NVIC_EnableIRQ(ADC_IRQn);							//habilito la interrupcion por adc
}


void confTimer0(void){
	TIM_MATCHCFG_Type confMatchTimer0 = {0};			//inicializo la struct con 0
	TIM_TIMERCFG_Type confPrescTimer0 = {0};

	confPrescTimer0.PrescaleOption 	  = TIM_PRESCALE_TICKVAL;
	confPrescTimer0.PrescaleValue	  = 500;			//10us con clk de timer a 100Mhz
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &confPrescTimer0);
	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER0, CLKPWR_PCLKSEL_CCLK_DIV_1);
	confMatchTimer0.MatchChannel	  = 1;				//MAT0.1
	confMatchTimer0.ResetOnMatch	  = 1;
	confMatchTimer0.StopOnMatch		  = 0;
	confMatchTimer0.MatchValue		  = 8;				//100us --> 10Khz de tasa de muestreo
	confMatchTimer0.IntOnMatch		  = ENABLE;			//habilito solo para comprobar que llega al match
	confMatchTimer0.ExtMatchOutputType	= TIM_EXTMATCH_TOGGLE;
	TIM_ConfigMatch(LPC_TIM0, &confMatchTimer0);
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void confDac(void){
	DAC_Init(LPC_DAC);  								//inicializo el dac en corriente maxima de 700uA y tiempo de respuesta 1us
}

void ADC_IRQHandler(void){
	uint16_t dac_value;
	adc_value = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_0);  //obtengo el valor convertido
	dac_value = adc_value >> 2; 							//como el dac es de 10 bit solo paso la parte mas alta del valor convertido de 12bit
	DAC_UpdateValue(LPC_DAC, dac_value);
}

void TIMER0_IRQHandler(void){
	TIM_ClearIntCapturePending(LPC_TIM0,TIM_MR1_INT);	    //limpio la bandera
	//LPC_GPIO0->FIOPIN ^= (1<<9);
}
