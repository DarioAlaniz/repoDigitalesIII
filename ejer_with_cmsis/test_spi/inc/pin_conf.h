/*
 * pin_conf.h
 *
 *  Created on: 31 oct. 2021
 *      Author: PC
 */

#ifndef PIN_CONF_H_
#define PIN_CONF_H_



#endif /* PIN_CONF_H_ */
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

#define PIN_EINT	10
#define PIN_CE_NRF24 1

#define PIN_CE_NRF24_RX 7

void confPin(void);
void confIntExt(void);


//TODO funciones auxiliar para probar modo RX
// Comentar cuando se tenga las 2 placas

void confIntExt1(void);
