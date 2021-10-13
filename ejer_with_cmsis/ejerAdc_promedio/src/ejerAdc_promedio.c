/*
===============================================================================
 Name        : ejerAdc_promedio.c
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
#include <stdio.h>
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_clkpwr.h"
#include "math.h"

#define BUFFER_ADDRES		0x10003000
#define BUFFER_SIZE			2048
#define BUFFER_FOR_CHANEL	BUFFER_SIZE/4
void confPin(void);
void confAdc(void);

uint16_t *p0 = (uint16_t*)BUFFER_ADDRES;
uint16_t *p1 = (uint16_t*)BUFFER_ADDRES+1;
uint16_t *p2 = (uint16_t*)BUFFER_ADDRES+2;
uint16_t *p3 = (uint16_t*)BUFFER_ADDRES+3;
uint8_t count0,count1,count2,count3=0; //lleva la cuenta de los datos guardados por cada canal
int main(void) {
	confPin();
	confAdc();

    while(1) {
		//reinicio los buffer
		if(count0 >= BUFFER_FOR_CHANEL)
			p0 = (uint16_t*)BUFFER_ADDRES;
		if(count1 >= BUFFER_FOR_CHANEL)
			p1 = (uint16_t*)BUFFER_ADDRES+1;
		if(count2 >= BUFFER_FOR_CHANEL)
			p2 = (uint16_t*)BUFFER_ADDRES+2;
		if(count3 >= BUFFER_FOR_CHANEL)
			p3 = (uint16_t*)BUFFER_ADDRES+3;
    }
    return 0 ;
}

void confPin(void){
	PINSEL_CFG_Type confPin= {0};
	confPin.Portnum			= 0;
	confPin.Pinmode			= 2; 	//ni pull up, ni pull dowm
	confPin.Funcnum			= 1;	//AD0.0 canal 0 del adc
	//configuro los 4 pines para los canales AD0.0,1,2,3
	for (uint8_t i=23;i<27;i++){
		confPin.Pinnum			= i;  	//P0.23,24,25,26
		PINSEL_ConfigPin(&confPin);
	}
}

void confAdc(void){
	//seteo el adc para que trabaje al maximo de sample 200Khz
	LPC_SC->PCONP	 |= (1<<12);						//power on, habilito el PCADC
	LPC_SC->PCLKSEL0 |= (3<<24);						//clk/8
	LPC_ADC->ADCR	 &= ~(0xFF<<8);						//clkdiv adc en 0

	ADC_PowerdownCmd(LPC_ADC, ENABLE);					//pongo en funcionamiento el ADC
	LPC_ADC->ADCR	 |= (0xF<<0);						//chanel 0,1,2,3 habilitados
	ADC_BurstCmd(LPC_ADC, ENABLE);
	LPC_ADC->ADINTEN |= (0xF<<0);						//interrupcion habilitadas para los 4 canales
	LPC_ADC->ADINTEN &= ~(1<<8);						//borro el bit de interrupciones globales

	NVIC_ClearPendingIRQ(ADC_IRQn);						// limpio la bandera
	NVIC_EnableIRQ(ADC_IRQn);							//habilito la interrupcion por adc
}

void ADC_IRQHandler(void){

	if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, 1)){
		*p0 = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);
		p0 +=4;
		count0++;
	}
	if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_1, 1)){
		*p1 = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_1);
		p1 +=4;
		count1++;
	}
	if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_2, 1)){
		*p2 = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_2);
		p2 +=4;
		count2++;
	}
	if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_3, 1)){
		*p3 = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_3);
		p3 +=4;
		count3++;
	}
}

/*
 * ptr posicion inicial del primer dato a sumar*/
uint32_t promedio(uint16_t* ptr){
	uint32_t prom = 0;
	for (uint8_t i = 0;i<=BUFFER_FOR_CHANEL;i++){
		prom += *ptr;
		ptr+=4;
	}
	return prom;
}














