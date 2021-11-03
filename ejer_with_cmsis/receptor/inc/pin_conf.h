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

#define EINT			0
#define PIN_EINT		10

#define PORT_CE_NRF24 	0
#define PIN_CE_NRF24 	1

#define PIN_SCK			15
#define PIN_MISO		17
#define PIN_MOSI		18

#define LED_RED_TOGGLE() 	LPC_GPIO0 -> FIOPIN ^= (1 << 22)
#define LED_BLUE_TOGGLE() 	LPC_GPIO3 -> FIOPIN ^= (1 << 26)
#define LED_GREEN_TOGGLE() 	LPC_GPIO3 -> FIOPIN ^= (1 << 25)

void confPin(void);
void confIntExt(void);
void initLEDPins(void);
