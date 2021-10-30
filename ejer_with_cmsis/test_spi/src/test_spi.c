/*
===============================================================================
 Name        : test_spi.c
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
#include "lpc17xx_ssp.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

#define CLK_SPI		2E6
#define LENGTH 		(uint8_t) 6
#define PIN_EINT	10

void confPin(void);
void confSpi(void);
void senDataSimple(uint8_t data);
void senData(SSP_DATA_SETUP_Type* pdataCfg);
void confIntExt(void);
void confTimer1 (void);
void delay_us (int time);


uint8_t buffer_dataTx[LENGTH];
uint8_t buffer_dataRx[LENGTH];
SSP_DATA_SETUP_Type dataCfg={0};


int main(void) {
	confPin();
	confSpi();

	dataCfg.length 	= LENGTH;
	dataCfg.tx_data = buffer_dataTx;
	dataCfg.rx_data = buffer_dataRx;

    while(1) {
    	senData(&dataCfg);

    }
    return 0 ;
}

void confPin(void){
	PINSEL_CFG_Type confPin = {0};
	confPin.Portnum	= 0;
	confPin.Pinnum 	= 15;	//P0.15
	confPin.Funcnum	= 2;    //SCK0
	PINSEL_ConfigPin(&confPin);
	confPin.Portnum	= 0;
	confPin.Pinnum 	= 16;	//P0.16
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
	confPin.Pinnum 	= 1;	//P0.1
	confPin.Funcnum	= 0;    //gpio --> CE (Chip Enable Activates RX or TX mode)
	confPin.Pinmode = 3; 	//pull down
	PINSEL_ConfigPin(&confPin);
	GPIO_SetDir(0, 1, 1);	//P0.1 output
}

void confSpi(void){
	SSP_CFG_Type confSpi;
	confSpi.Databit   	= SSP_DATABIT_8;
	confSpi.FrameFormat = SSP_FRAME_SPI;
	confSpi.Mode		= SSP_MASTER_MODE;
	confSpi.CPHA 		= SSP_CPHA_FIRST;
	confSpi.CPOL 		= SSP_CPOL_LO;
	confSpi.ClockRate	= CLK_SPI;
	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_SSP0,CLKPWR_PCLKSEL_CCLK_DIV_1); //100Mhz
	SSP_Init(LPC_SSP0, &confSpi);
	SSP_Cmd(LPC_SSP0, ENABLE);

}

void RF_CSN_HIGH(){
	LPC_GPIO0->FIOSET |=(1<<16);  //high para desabilitar el modulo
}

void RF_CSN_LOW(){
	LPC_GPIO0->FIOCLR |= (1<<16); //low para habilitar el modulo
}


void senDataSimple(uint8_t data){
	RF_CSN_LOW();
	while(!(SSP_GetStatus(LPC_SSP0, SSP_STAT_BUSY))){
		SSP_SendData(LPC_SSP0, data);
	};
	RF_CSN_HIGH();
}

/*
 * envia un buffer de datos y recive la misma cantidad de datos que envio*/
void senData(SSP_DATA_SETUP_Type* pdataCfg){
	RF_CSN_LOW();
	SSP_ReadWrite(LPC_SSP0, pdataCfg, SSP_TRANSFER_POLLING);
	RF_CSN_HIGH();
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
	Eint0Cfg.EXTI_polarity	= 1;   		//flanco de subida
	EXTI_Config(&Eint0Cfg);				//configuro la interrupcion, limpia la bandera
	NVIC_SetPriority(EINT0_IRQn, 2);	//
	NVIC_EnableIRQ(EINT0_IRQn); 		//habilito la interrupcion
	return;
}

void confTimer1 (void) {
	TIM_TIMERCFG_Type timer1_conf;

	timer1_conf.PrescaleOption = TIM_PRESCALE_USVAL;
	timer1_conf.PrescaleValue  = 1000; //resolucion de 1ms (si PCLK= 25MHz)

	TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &timer1_conf); //TIM_Init no le da enable
}

void delay_us (int time) {
	TIM_ResetCounter(LPC_TIM1);
	TIM_Cmd(LPC_TIM1, ENABLE);
	while (LPC_TIM1 -> TC < time); //en [us]
	TIM_Cmd(LPC_TIM1, DISABLE);
}
