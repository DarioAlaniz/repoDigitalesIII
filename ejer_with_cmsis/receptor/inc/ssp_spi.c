/*
 * spi.c
 *
 *  Created on: 31 oct. 2021
 *      Author: PC
 */

#include "LPC17xx.h"
#include "lpc17xx_ssp.h"
#include "ssp_spi.h"
#include "lpc17xx_clkpwr.h"
#include "timers.h"


void confSpi(void){
	SSP_CFG_Type confSpi;
	confSpi.Databit   	= SSP_DATABIT_8;
	confSpi.FrameFormat = SSP_FRAME_SPI;
	confSpi.Mode		= SSP_MASTER_MODE;
	confSpi.CPHA 		= SSP_CPHA_FIRST;
	confSpi.CPOL 		= (0<<6);
	confSpi.ClockRate	= CLK_SPI;
	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_SSP1,CLKPWR_PCLKSEL_CCLK_DIV_1); //100Mhz
	SSP_Init(LPC_SSP_SEL, &confSpi);
	SSP_Cmd(LPC_SSP_SEL, ENABLE);
	LPC_SSP_SEL->DR = 0; //limpio lo que tenga el registro
}

void senDataSimple(uint8_t data){
	while(!(SSP_GetStatus(LPC_SSP_SEL, SSP_STAT_BUSY))){
		SSP_SendData(LPC_SSP_SEL, data);
	};
}

/*
 * envia un buffer de datos y recive la misma cantidad de datos que envio*/
void senData(SSP_DATA_SETUP_Type* pdataCfg){
	SSP_ReadWrite(LPC_SSP_SEL, pdataCfg, SSP_TRANSFER_POLLING);
}

uint16_t recData(void){
	senDataSimple(0xFF);
	delay_us(10);
	return SSP_ReceiveData(LPC_SSP_SEL);
}

void clearRxFifo(){
	uint32_t tmp;
	while (LPC_SSP_SEL->SR & SSP_SR_RNE){
		tmp = (uint32_t) SSP_ReceiveData(LPC_SSP_SEL);
	}
}


void RF_CSN_HIGH(){
	LPC_GPIO0->FIOSET |=(1<<PIN_SSEL);  //high para desabilitar el modulo
}

void RF_CSN_LOW(){
	LPC_GPIO0->FIOCLR |= (1<<PIN_SSEL); //low para habilitar el modulo
}

