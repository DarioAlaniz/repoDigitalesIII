/*
 * spi.h
 *
 *  Created on: 31 oct. 2021
 *      Author: PC
 */

#ifndef SSP_SPI_H_
#define SSP_SPI_H_


#endif /* SSP_SPI_H_ */

#include "lpc17xx_ssp.h"


#define CLK_SPI		2E6
#define PIN_SSEL0	16
#define PIN_SSEL1   6


#define LPC_SSP_SEL LPC_SSP0
#define PIN_SSEL	PIN_SSEL0




void confSpi(void);
void RF_CSN_LOW();
void RF_CSN_HIGH();
void senDataSimple(uint8_t data);
void senData(SSP_DATA_SETUP_Type* pdataCfg);
uint16_t recData(void);
void clearRxFifo();

