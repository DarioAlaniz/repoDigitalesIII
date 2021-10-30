/*
 * nRF24l01.c
 *
 *  Created on: 30 oct. 2021
 *      Author: PC
 */


#include "nRF24L01.h"


uint8_t WriteToNrf(uint8_t ReadWrite,uint8_t reg, uint8_t *val,uint8_t length){
	if(ReadWrite='W'){
		reg = RF24_W_REGISTER + reg;
	}

}
