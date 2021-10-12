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

#define FREQ_SAMPLE		200000

void confPin(void);
void confAdc(void);
void confTimer0(void);
void confDac(void);

uint16_t adc_buffer [520];			//buffer circular para ver si hay error en la medicion
uint16_t length_buffer = sizeof(adc_buffer)/sizeof(adc_buffer[0]);
uint32_t adc_value;
uint32_t dac_value;
uint16_t dac_value_test = 0;
uint8_t flag_timer;
uint8_t flag_adc;

int main(void) {
	SystemInit();
	confPin();
	confTimer0();
	confDac();
	confAdc();
	TIM_Cmd(LPC_TIM0, ENABLE);							//habilito el timer
	TIM_ResetCounter(LPC_TIM0);							//reset el timer

    while(1) {
//    	if(flag_adc){
//    		flag_adc = 0;
//    		for (int i=length_buffer-1;i>0;i--){
//    			adc_buffer[i] = adc_buffer[i-1];
//    		}
//    		adc_buffer[0] =adc_value;
//    	}
    	if(flag_timer){
    		flag_timer = 0;
//    		dac_value  = dac_value_test ;
//    		dac_value_test++;
//    		if (dac_value_test == 1024) dac_value_test = 0;
    		if ((adc_value >> 2) >= 1024 ){
    			// no actualizo adc value hay ruido en la señal
    			dac_value = dac_value;
    		}
    		else {
    			dac_value	= (adc_value>>2);
				DAC_UpdateValue(LPC_DAC, dac_value);
    		}
    		/*al pasar un valor directo del adc aparece un pico en la salida del dac,
    		 * da a entender que se satura la salida del dac pero no deberia ya que el adc esta leyendo
    		 * un valor fijo de 1v, investigando se llego a que es un problema del adc y se requiere un filtro pasa bajo
    		 * en la entrada del adc!!!
    		 * */
    	}
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
	confPin.Pinnum			= 9;	//P0.9
	confPin.Pinmode			= 0; 	//pull up
	confPin.Funcnum			= 0;	//Gpio
	PINSEL_ConfigPin(&confPin);
	GPIO_SetDir(0,(1<<9),1);		//output
}

void confAdc(void){
	/* cmsis setea mal el divisor de clk del adc para la frequencia maxima
	 * si clk/4 = 25Mhz --> cmsis establece este valor al adc
	 * si clk/8 = 12.5Mhz --> cmsis establece un valor negativo en divisor interno del adc
	 * si clk/2 = 50Mhz --> cmsis establece un valor de 2 al divisor interno
	 * si clk/1 = 100Mhz --> cmsis establece un valor de 6 al divisor interno 16.66Mhz
	 *
	 * En todas las configuracion posibles para la frecuencia de muestreo maxima cmsis
	 * setea mal el valor, por lo que seria mejor hacerlo con punteros.
	 */
//	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_ADC, 1); 			//clk/2
//	ADC_Init(LPC_ADC, FREQ_SAMPLE); 					//power on, habilito el PCADC

	LPC_SC->PCONP	 |= (1<<12);						//power on, habilito el PCADC
	LPC_SC->PCLKSEL0 |= (3<<24);						//clk/8
	LPC_ADC->ADCR	 &= ~(0xFF<<8);						//clkdiv adc en 0

	ADC_PowerdownCmd(LPC_ADC, ENABLE);					//pongo en funcionamiento el ADC
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE); 	//habilito solo el canal 0
	ADC_BurstCmd(LPC_ADC, ENABLE);
//	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01);			//habilito el adc para cuando hace match con el timer0
//	ADC_EdgeStartConfig(LPC_ADC, 1);
	ADC_IntConfig(LPC_ADC,ADC_ADGINTEN,RESET);			//desabilito la interrup global que por defecto es 1
	ADC_IntConfig(LPC_ADC,ADC_ADINTEN0,ENABLE);			//habilito la interrupcion para el canal 0
	LPC_ADC->ADDR0	&= LPC_ADC->ADDR0;					//limpio la badera de interrupcion leyendo el registro
	NVIC_ClearPendingIRQ(ADC_IRQn);						// limpio la bandera
	NVIC_EnableIRQ(ADC_IRQn);							//habilito la interrupcion por adc
}


void confTimer0(void){
	TIM_MATCHCFG_Type confMatchTimer0 = {0};			//inicializo las struct
	TIM_TIMERCFG_Type confPrescTimer0 = {0};

	confPrescTimer0.PrescaleOption 	  = TIM_PRESCALE_TICKVAL;
	confPrescTimer0.PrescaleValue	  = 50;				//1us con clk de timer a 100Mhz
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &confPrescTimer0);
	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER0, CLKPWR_PCLKSEL_CCLK_DIV_1);
	confMatchTimer0.MatchChannel	  = 1;				//MAT0.1
	confMatchTimer0.ResetOnMatch	  = 1;
	confMatchTimer0.MatchValue		  = 8;				//5us --> 200Khz de tasa de muestreo
//	confMatchTimer0.IntOnMatch		  = ENABLE;			//habilito solo para comprobar que llega al match
	confMatchTimer0.ExtMatchOutputType	= TIM_EXTMATCH_TOGGLE;
	TIM_ConfigMatch(LPC_TIM0, &confMatchTimer0);

	TIM_ClearIntPending(LPC_TIM0, TIM_MR1_INT);	    //limpio la bandera
	NVIC_ClearPendingIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void confDac(void){
	DAC_Init(LPC_DAC);  								//inicializo el dac en corriente maxima de 700uA y tiempo de respuesta 1us
//	DAC_SetBias(LPC_DAC,1);
	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_DAC, CLKPWR_PCLKSEL_CCLK_DIV_1);
}

void ADC_IRQHandler(void){
//	LPC_GPIO0->FIOPIN ^= (1<<9);
//	flag_adc  = 1;
	adc_value = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_0);  //obtengo el valor convertido
	dac_value	= (adc_value>>2);
	DAC_UpdateValue(LPC_DAC, dac_value);
}

void TIMER0_IRQHandler(void){
	flag_timer = 1;
	TIM_ClearIntPending(LPC_TIM0, TIM_MR1_INT);	    //limpio la bandera
}
