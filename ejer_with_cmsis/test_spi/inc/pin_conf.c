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
	confPin.Pinnum 	= 15;	//P0.15
	confPin.Funcnum	= 2;    //SCK0
	PINSEL_ConfigPin(&confPin);
	confPin.Portnum	= 0;
	confPin.Pinnum 	= PIN_SSEL0;	//P0.16
	confPin.Funcnum	= 0;    //Gpio--->SSEL0 se hace por soft por que por hardware pone en bajo y alto por cada byte enviado
							//y el modulo necesita que este siempre en bajo acta que se terminen de enviar todos los bytes
	GPIO_SetDir(0, (1<<16), 1);	//P0.16 output
	PINSEL_ConfigPin(&confPin);
	confPin.Portnum	= 0;
	confPin.Pinnum 	= 17;	//P0.17
	confPin.Funcnum	= 2;    //MISO0
	PINSEL_ConfigPin(&confPin);
	confPin.Portnum	= 0;
	confPin.Pinnum 	= 18;	//P0.18
	confPin.Funcnum	= 2;    //MOSI0
	PINSEL_ConfigPin(&confPin);
	confPin.Portnum	= 0;
	confPin.Pinnum 	= PIN_CE_NRF24;	//P0.1
	confPin.Funcnum	= 0;    //gpio --> CE (Chip Enable Activates RX or TX mode)
	confPin.Pinmode = 3; 	//pull down
	PINSEL_ConfigPin(&confPin);
	GPIO_SetDir(0, (1<<PIN_CE_NRF24), 1);	//P0.1 output

	//TODO funciones auxiliar para probar modo RX
	// Comentar cuando se tenga las 2 placas
	/*******************************************************************************************************/
	confPin.Portnum	= 0;
	confPin.Pinnum 	= 6;	//P0.6
	confPin.Funcnum	= 0;    //Gpio--->SSEL1 RX
	GPIO_SetDir(0, (1<<PIN_SSEL1), 1);	//P0.6 output
	PINSEL_ConfigPin(&confPin);
	confPin.Portnum	= 0;
	confPin.Pinnum 	= PIN_CE_NRF24_RX;	//P0.7
	confPin.Funcnum	= 0;    //gpio --> CE (Chip Enable Activates RX or TX mode)
	confPin.Pinmode = 3; 	//pull down
	PINSEL_ConfigPin(&confPin);
	GPIO_SetDir(0, (1<<PIN_CE_NRF24_RX), 1);	//P0.7 output
	/*******************************************************************************************************/

}

void confIntExt(void){
	PINSEL_CFG_Type pinCfg;
	pinCfg.Portnum 	= 2;
	pinCfg.Pinnum	= 10;  				//P2.10
	pinCfg.Funcnum	= 1;				//EINT0
	pinCfg.Pinmode	= 0;				//pull-up
	PINSEL_ConfigPin(&pinCfg);

	EXTI_InitTypeDef Eint0Cfg;
	Eint0Cfg.EXTI_Line 		= 0; 		//EINT0
	Eint0Cfg.EXTI_Mode		= 1;		//activo por flanco
	Eint0Cfg.EXTI_polarity	= 0;   		//flanco de bajada
	EXTI_Config(&Eint0Cfg);				//configuro la interrupcion, limpia la bandera
	NVIC_SetPriority(EINT0_IRQn, 2);	//
	NVIC_EnableIRQ(EINT0_IRQn); 		//habilito la interrupcion
	return;
}

//TODO funciones auxiliar para probar modo RX
// Comentar cuando se tenga las 2 placas
/*******************************************************************************************************/
void confIntExt1(void){
	PINSEL_CFG_Type pinCfg;
	pinCfg.Portnum 	= 2;
	pinCfg.Pinnum	= 11;  				//P2.11
	pinCfg.Funcnum	= 1;				//EINT1
	pinCfg.Pinmode	= 0;				//pull-up
	PINSEL_ConfigPin(&pinCfg);

	EXTI_InitTypeDef Eint0Cfg;
	Eint0Cfg.EXTI_Line 		= 1; 		//EINT0
	Eint0Cfg.EXTI_Mode		= 1;		//activo por flanco
	Eint0Cfg.EXTI_polarity	= 0;   		//flanco de bajada
	EXTI_Config(&Eint0Cfg);				//configuro la interrupcion, limpia la bandera
	NVIC_SetPriority(EINT1_IRQn, 3);	//
	NVIC_EnableIRQ(EINT1_IRQn); 		//habilito la interrupcion
	return;
}

/*******************************************************************************************************/
