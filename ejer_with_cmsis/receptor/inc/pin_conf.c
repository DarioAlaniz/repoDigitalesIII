/*
 * pin_conf.c
 *
 *  Created on: 31 oct. 2021
 *      Author: PC
 */

#include "lpc17xx_exti.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "pin_conf.h"
#include "ssp_spi.h"

void confPin(void){
	PINSEL_CFG_Type confPin = {0};
	confPin.Portnum	= 0;
	confPin.Pinnum 	= PIN_SCK;	//P0.PIN_SCK
	confPin.Funcnum	= 2;    	//SCK
	PINSEL_ConfigPin(&confPin);
	confPin.Portnum	= 0;
	confPin.Pinnum 	= PIN_SSEL;	//P0.PIN_SSEL
	confPin.Funcnum	= 0;    //Gpio--->SSEL0 se hace por soft por que por hardware pone en bajo y alto por cada byte enviado
							//y el modulo necesita que este siempre en bajo acta que se terminen de enviar todos los bytes
	GPIO_SetDir(0, (1<<PIN_SSEL), 1);	//P0.PIN_SSEL output
	PINSEL_ConfigPin(&confPin);
	confPin.Portnum	= 0;
	confPin.Pinnum 	= PIN_MISO;	//P0.PIN_MISO
	confPin.Funcnum	= 2;    	//MISO
	PINSEL_ConfigPin(&confPin);
	confPin.Portnum	= 0;
	confPin.Pinnum 	= PIN_MOSI;	//P0.PIN_MOSI
	confPin.Funcnum	= 2;    	//MOSI0
	PINSEL_ConfigPin(&confPin);
	confPin.Portnum	= PORT_CE_NRF24;
	confPin.Pinnum 	= PIN_CE_NRF24;	//PPORT_CE_NRF24.PIN_CE_NRF24
	confPin.Funcnum	= 0;    //gpio --> CE (Chip Enable Activates RX or TX mode)
	confPin.Pinmode = 3; 	//pull down
	PINSEL_ConfigPin(&confPin);
	GPIO_SetDir(0, (1<<PIN_CE_NRF24), 1);	//P0.1 output

}

void confIntExt(void){
	PINSEL_CFG_Type pinCfg;
	pinCfg.Portnum 	= 2;
	pinCfg.Pinnum	= PIN_EINT;  				//P2.PIN_EINT
	pinCfg.Funcnum	= 1;				//EINTx
	pinCfg.Pinmode	= 0;				//pull-up
	PINSEL_ConfigPin(&pinCfg);

	EXTI_InitTypeDef EintCfg;
	EintCfg.EXTI_Line 		= EINT; 		//EINTX
	EintCfg.EXTI_Mode		= 1;			//activo por flanco
	EintCfg.EXTI_polarity	= 0;   			//flanco de bajada
	EXTI_Config(&EintCfg);					//configuro la interrupcion, limpia la bandera
	uint8_t typeEint;
	switch(EINT){
	case 0:
		typeEint = EINT0_IRQn;
		break;
	case 1:
		typeEint = EINT1_IRQn;
		break;
	case 2:
		typeEint = EINT2_IRQn;
		break;
	case 3:
		typeEint = EINT3_IRQn;
		break;
	}
	NVIC_SetPriority(typeEint, 3);		//CONFIGURAR A GUSTO!!
	NVIC_EnableIRQ(typeEint); 			//habilito la interrupcion
	return;
}


void initLEDPins(void) {
	/*
	 * P0.22 Red
	 * P3.26 Blue
	 * P3.25 Green
	 */
	const uint8_t ledNum[] = {22, 26, 25};
	const uint8_t ledPort[] = {0, 3, 3};

	PINSEL_CFG_Type ledPin;
	ledPin.Pinmode = PINSEL_PINMODE_PULLUP;
	ledPin.Funcnum = 0;


	for (int i = 0;i < 3; i++) {
		ledPin.Pinnum = ledNum[i];
		ledPin.Portnum = ledPort[i];
		PINSEL_ConfigPin(&ledPin);
		GPIO_SetDir(ledPort[i], 1<<ledNum[i], 1);
	}

	//dejar pines en alto (LEDS OFF)
	GPIO_SetValue(0, 1<<22);
	GPIO_SetValue(3, 1<<25);
	GPIO_SetValue(3, 1<<26);
}
