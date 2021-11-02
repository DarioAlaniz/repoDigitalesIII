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

#include "pin_conf.h"
#include "ssp_spi.h"
#include "timers.h"
#include "nRF24L01.h"
#include "lpc17xx_exti.h"

#define BUFFER_LENGTH  PAYLOAD_WIDTH

void checkData();


uint8_t val0[5];
uint8_t val1[5];

uint8_t status;
uint8_t conf[1];
uint8_t buffer_tx[BUFFER_LENGTH];//= {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A};
uint8_t receiveData[BUFFER_LENGTH];
uint8_t count_loss[1] = {0};
uint8_t count_error;
int main(void) {
	confPin();
	confSpi();
	confTimer1();
	confTimer2();
	//status = nrf24_status();
	nrf24_init(1);
	nrf24_writeToNrf(R, RF24_FIFO_STATUS, val0, 1);
	//nrf24_writeToNrf(R, RF24_TX_ADDR, val0, BUFFER_LENGTH);
	//status = nrf24_status_RX();
	nrf24_init_RX(0);
	nrf24_writeToNrf_RX(R, RF24_FIFO_STATUS, val0, 1);
	confIntExt();
	confIntExt1();
	for(int i =0;i<BUFFER_LENGTH;i++){
			buffer_tx[i] = i;
		}
	nrf24_listen_payload();
	nrf24_transmit_payload(buffer_tx,BUFFER_LENGTH);
	checkData();
    while(1) {

    }
    return 0 ;
}

void EINT0_IRQHandler(void) {
	uint8_t aux0[1];
	EXTI_ClearEXTIFlag(0);
	uint8_t status= nrf24_status();
	if(status & RF24_MASK_MAX_RT){
		//error de retransmicion
		nrf24_writeToNrf(R, RF24_OBSERVE_TX, count_loss , sizeof(count_loss));
		aux0[0] = status | RF24_MASK_MAX_RT;
		nrf24_writeToNrf(W, RF24_STATUS, aux0 , sizeof(aux0)); //limpio el flag MAX_RT (miximo de retrasnmiciones)
	}
	else if(status & (RF24_MASK_TX_DS|RF24_MASK_RX_DR)){
		//transmicion exitosa
		aux0[0] = status | RF24_MASK_TX_DS | RF24_MASK_RX_DR;
		nrf24_writeToNrf(W, RF24_STATUS, aux0 , sizeof(aux0)); //limpio el flag TX_DS y RX_DR
	}
}

void EINT1_IRQHandler (void) {
    uint8_t aux0[1];
    EXTI_ClearEXTIFlag(1);
    nrf24_CE_low_RX();
    uint8_t status = nrf24_status_RX();
    if(status & RF24_MASK_RX_DR) {
        aux0[0] = status | RF24_MASK_TX_DS | RF24_MASK_RX_DR;
        nrf24_writeToNrf_RX(R, RF24_R_RX_PAYLOAD, receiveData, sizeof(receiveData)); //leo la FIFO
        nrf24_writeToNrf_RX(W, RF24_STATUS, aux0, sizeof(aux0)); //limpio los flags
    }
    nrf24_CE_high_RX();
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
}

