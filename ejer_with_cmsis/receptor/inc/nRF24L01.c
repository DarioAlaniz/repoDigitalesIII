/*
 * nrF24L001.c
 *
 *  Created on: 31 oct. 2021
 *      Author: PC
 *      https://github.com/ErichStyger/mcuoneclipse/blob/master/Examples/CodeWarrior/FRDM-KL25Z/Freedom_NRF24L01/Sources/nRF24L01.h //define obtenidos
 *      https://gizmosnack.blogspot.com/2013/04/tutorial-nrf24l01-and-avr.html //configuracion utilizada
 */


#include "LPC17xx.h"
#include "ssp_spi.h"
#include "timers.h"
#include "pin_conf.h"
#include <nRF24L01.h>

void nrf24_writeToNrf (uint8_t readWrite, uint8_t reg, uint8_t *valArray, uint8_t length) {

	//static uint8_t ret[32];
/*
 * posible forma de implementar la lectura y escritura de buffer
 * pero no tiene un control de tiempos, siempre envia y recive siempre y cuando
 * el buffer de salida o entreda este libre o lleno respectivamente
 * SSP_DATA_SETUP_Type dataCfg;
 * dataCfg.length  = length;
 * dataCfg.tx_data = valArray;
 * dataCfg.rx_data = ret;
*/

	/* Siempre hay que enviar un array en como valor */
	if (readWrite == W)
		reg = RF24_W_REGISTER + reg;

	RF_CSN_LOW();
	delay_us(10);
	senDataSimple(reg);
	delay_us(10);
	clearRxFifo();		//limpia la Rxfifo del bus spi

	for (uint8_t i = 0; i < length; i++) {
		if (readWrite == R && reg != RF24_W_TX_PAYLOAD){
			valArray[i] = recData();
			delay_us(10);
		}
		else{
			senDataSimple(valArray[i]);
			delay_us(10);
		}
	}

	RF_CSN_HIGH();

	//return ret;
}


uint8_t nrf24_status(){
	clearRxFifo();
	RF_CSN_LOW();
	uint8_t status = recData();
	RF_CSN_HIGH();
	delay_us(10);
	return status;
}
/* inicializa el modulo en funcion de tx_rx
 * addres por defecto {0x87, 0x33, 0xC3, 0x1F, 0xD6}
 * @param tx_rx: 1 TX Mode
 * 				 0 RX Mode
 * 				 */
void nrf24_init(uint8_t tx_rx) {
	delay_ms(100);

	uint8_t value[1];
	uint8_t rfAddr[5] = {0x87, 0x33, 0xC3, 0x1F, 0xD6}; //misma para tx y rx (5 bytes) en AA mode
	uint8_t *p = value;
	//EN_AA
	*p = RF24_EN_AA_ENAA_P0;
	nrf24_writeToNrf(W, RF24_EN_AA, value, sizeof(value));
	//Enable data pipe 0
	*p = RF24_EN_RXADDR_ERX_P0;
	nrf24_writeToNrf(W, RF24_EN_RXADDR, value, sizeof(value));
	//RF address width setup
	*p = RF24_SETUP_AW_5BYTES;
	nrf24_writeToNrf(W, RF24_SETUP_AW, value, sizeof(value));
	//RF channel freq
	*p = RF24_RF_CH_DEFAULT_VAL;
	nrf24_writeToNrf(W, RF24_RF_CH, value, sizeof(value));
	//RF power mode
	*p = 0x07;
	nrf24_writeToNrf(W, RF24_RF_SETUP, value, sizeof(value));
	//RX RF addr setup

	nrf24_writeToNrf(W, RF24_RX_ADDR_P0, rfAddr, sizeof(rfAddr));
	if(tx_rx == W){
		nrf24_writeToNrf(W, RF24_TX_ADDR, rfAddr, sizeof(rfAddr)); //Esta es sola para el transmisor
		*p = 0x04; //4 bytes (32 bits);
	}
	else{
		//Payload width setup
		*p = PAYLOAD_WIDTH; //PAYLOAD_WIDTH bytes (32 bits);
	}
	nrf24_writeToNrf(W, RF24_RX_PW_P0, value, sizeof(value));
	//Config register
	//bit 7 = reserv, bit 6 = MASK_RX_DR,bit 5 = MASK_TX_DS,bit 4 = MASK_MAX_RT,
	//bit 3 = EN_CRC, bit 2 = CRCO,      bit 1 =PWR_UP,     bit 0 = PRIM_RX.
	if(tx_rx == W)
		*p= 0b00001110; //solo TX
	else
		*p= 0b00111111; //solo RX
	nrf24_writeToNrf(W, RF24_CONFIG, value, sizeof(value)); //como transmisor
	if(tx_rx == W){
		//sets the number of retries and the retry delay like this
		*p = 0x2F; 		//0b0010_0011 "2" sets it up to 750uS delay between every retry (at least 500us at 250jbps and if payload >5bytes in 1Mbps,
						//and if payload >15byte in 2Mbps) 'F' is number of retries (1-15, now 15)
		nrf24_writeToNrf(W,RF24_SETUP_RETR,value,1);
	}

	*p= RF24_STATUS_DEFAULT_VAL | RF24_MASK_MAX_RT | RF24_MASK_TX_DS | RF24_MASK_RX_DR; //limpio el status
	nrf24_writeToNrf(W,RF24_STATUS,value,1);

	delay_ms(100);
}

void nrf24_transmit_payload(uint8_t * w_buffer,uint8_t length){
	nrf24_writeToNrf(R, RF24_FLUSH_TX, w_buffer, 0); //limpia la fifoTX
	nrf24_writeToNrf(R, RF24_W_TX_PAYLOAD, w_buffer, length);
	delay_ms(10);
	nrf24_CE_high();
	delay_us(160);
	nrf24_CE_low();
	delay_ms(10);
}


void nrf24_CE_high(void){
	LPC_GPIO0->FIOSET |= (1<<PIN_CE_NRF24);
}

void nrf24_CE_low(void){
	LPC_GPIO0->FIOCLR |= (1<<PIN_CE_NRF24);
}

void nrf24_listen_payload(void){
	nrf24_CE_high();
	delay_us(150);				// tiempo que necesita para ponerse en modo RX
}
