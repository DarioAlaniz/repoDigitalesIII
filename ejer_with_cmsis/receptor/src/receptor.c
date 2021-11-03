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
#include "lpc17xx_exti.h"

#define BUFFER_LENGTH  PAYLOAD_WIDTH

uint8_t val[5];
uint8_t buffer_tx[BUFFER_LENGTH];
uint8_t receiveData[BUFFER_LENGTH];
uint8_t count_error;
uint8_t status;
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
	nrf24_writeToNrf(R, RF24_RX_ADDR_P0, val, 5);
	nrf24_init(0);
	nrf24_writeToNrf(R, RF24_RX_ADDR_P0, val, 5);
	status = nrf24_status();
	nrf24_writeToNrf(R, RF24_CONFIG, val, 1);
	nrf24_listen_payload();
    while(1) {

    }
    return 0 ;
}


void EINT0_IRQHandler (void) {
    uint8_t aux0[1];
    EXTI_ClearEXTIFlag(0);
    nrf24_CE_low();
    uint8_t status = nrf24_status();
    if(status & RF24_MASK_RX_DR) {
        aux0[0] = status | RF24_MASK_TX_DS | RF24_MASK_RX_DR;
        nrf24_writeToNrf(R, RF24_R_RX_PAYLOAD, receiveData, sizeof(receiveData)); //leo la FIFO
        nrf24_writeToNrf(W, RF24_STATUS, aux0, sizeof(aux0)); //limpio los flags
        checkData();
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
