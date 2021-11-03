/*
===============================================================================
 Name        : receptor.c
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
#include "pin_conf.h"
#include "ssp_spi.h"
#include "timers.h"
#include "nRF24L01.h"
#include "adc_dac.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_dac.h"



#define BUFFER_LENGTH  PAYLOAD_WIDTH
#define KEY			   0xAA


uint8_t val[5];
uint8_t buffer_tx[BUFFER_LENGTH];
uint8_t receiveData[BUFFER_LENGTH];
uint8_t count_error;
uint8_t status;
uint8_t flag_recv_data;
uint16_t dac_value;
void checkData();
int main(void) {
	for(int i =0;i<BUFFER_LENGTH;i++){
		buffer_tx[i] = i;
	}
	confPin();
	initLEDPins();
	confSpi();
	confIntExt();
	confTimer1();
	confTimer2();
	//initRit();
	initDAC();
	nrf24_init(0);

	//habilito modulo en RX
	nrf24_listen_payload();
	LED_BLUE_TOGGLE();
    while(1) {
    	if(flag_recv_data){
    		flag_recv_data = 0;
    		if(receiveData[0] == KEY){
    			dac_value = (((uint16_t)receiveData[1]) << 8) | (receiveData[2]);
    			DAC_UpdateValue(LPC_DAC, (dac_value>>2)&0x3FF);
    		}
    		LED_BLUE_TOGGLE();
			LED_GREEN_TOGGLE();
    	}
    }
    return 0 ;
}


void EINT0_IRQHandler (void) {
    uint8_t aux0[1];
    EXTI_ClearEXTIFlag(0);
    nrf24_CE_low();
    uint8_t status = nrf24_status();
    if(status & RF24_MASK_RX_DR) {
    	flag_recv_data = 1;
        aux0[0] = status | RF24_MASK_TX_DS | RF24_MASK_RX_DR;
        nrf24_writeToNrf(R, RF24_R_RX_PAYLOAD, receiveData, sizeof(receiveData)); //leo la FIFO
        nrf24_writeToNrf(W, RF24_STATUS, aux0, sizeof(aux0)); //limpio los flags
        LED_BLUE_TOGGLE();
        LED_GREEN_TOGGLE();
        //checkData();
    }
    else {
    	LED_RED_TOGGLE();
    }
    nrf24_CE_high();
}


void checkData(){
	count_error=0;

	for(int i=0;i<BUFFER_LENGTH;i++){
		if(buffer_tx[i]!=receiveData[i]){
			count_error++;
		}
		else {
			//
		}
	}
	if(count_error){
		LED_RED_TOGGLE();
	}
	else {
		LED_GREEN_TOGGLE();
	}
}

