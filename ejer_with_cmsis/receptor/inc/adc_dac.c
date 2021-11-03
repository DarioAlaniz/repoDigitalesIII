/*
 * adc_dac.c
 *
 *  Created on: 3 nov. 2021
 *      Author: PC
 */
#include "LPC17xx.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_pinsel.h"

void initDAC(void){
	//p0.26 a AOUT (habilitar acceso a registros)
	PINSEL_CFG_Type cfgP026={
		.OpenDrain=PINSEL_PINMODE_NORMAL,
		.Pinmode=PINSEL_PINMODE_TRISTATE,
		.Funcnum=2,
		.Pinnum=26,
		.Portnum=0
	};
	PINSEL_ConfigPin(&cfgP026);

	//iniciar dac, maxima velocidad 1MHz
	DAC_Init(LPC_DAC);
	DAC_SetBias(LPC_DAC, 0);

	return;
}
