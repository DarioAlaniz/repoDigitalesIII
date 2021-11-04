/*
===============================================================================
 Name        : receptor.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition

 PINES:
 	 ...

 TODO: CONFIGURAR DELAY TRANSMIT PAYLOAD
 TODO: Payload de menos bytes?
 TODO: cambie los blink leds (parametro int)
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
#include "LPC17xx.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_rit.h"
#include <cr_section_macros.h>

//Macros UART
#define MULVAL 			14
#define DIVADDVAL 		2
#define Ux_FIFO_EN 		(1<<0)
#define Rx_FIFO_RST 	(1<<1)
#define Tx_FIFO_RST 	(1<<2)
#define DLAB_BIT 		(1<<7)
#define Rx_RDA_INT 		(1<<0)
#define MASK_INT_ID 	(0xE)
#define MASK_RDA_INT 	(2)
#define UARTx 			(LPC_UART_TypeDef*) LPC_UART2_BASE

//Modulo Tx
#define COUNT_ERR_TRG 	30
#define BUFFER_LENGTH 	PAYLOAD_WIDTH

uint8_t buffer_tx[BUFFER_LENGTH];
uint8_t receiveData[BUFFER_LENGTH];
uint8_t count_error = 0;
uint8_t currentKey = 0xF1;
uint8_t readyToLoad = 1;
uint8_t FlagErrorTransmission = 0;
uint8_t cmd[2];

//Var de ADC + filtro prom
volatile uint32_t 	acc0=0;
volatile uint8_t 	samp=0;
volatile uint8_t 	n=8; //TODO: PUEDE FALLAR xd
volatile uint16_t 	res0=0;
volatile uint16_t 	dacval=0;

//proto uart
void initUARTx(void);
void initUART2(void);
//proto gpio
void blinkRedLed(int);
void blinkGreenLed(int);
void blinkBlueLed(int);
void initLEDPins(void);
//proto ADC
void initADC(void);
//proto Timers
void initTMR0(void);
void initTMR2(void);


int main(void) {
	/* System Clock Init */
	SystemInit();
	/* Init */
	initLEDPins();
//	initUARTx();
	initUART2();
	initADC();
	initTMR0(); // TODO: OJO modificar para Fs 8k
	confTimer1();
	confTimer2();
	confPin();
	initLEDPins();
	confSpi();
	nrf24_init(1);	//Modo TX-con auto acknowledge
	confIntExt();

	/* Algorithm */
	while(1) {
		//TODO: lo nuevo
		if(FlagErrorTransmission) {
			//Error: turn off radio
			TIM_Cmd(LPC_TIM0, DISABLE);
			FlagErrorTransmission = 0;
			blinkRedLed(1000);
			delay_ms(1000);
			blinkRedLed(1000);
			delay_ms(1000);
			blinkRedLed(1000);
		}
//		UART_Receive(UARTx, (uint8_t *)cmd, sizeof(cmd), BLOCKING);
		/*
		 * Decide que comando ejecutar:
		 * '.g': Get current Key
		 * '.s': Set a new Key
		 * '.r': Record and send message
		 * '.o': Turn on radio
		 * '.f': Turn off radio
		 */
		switch (cmd[0]) {
		//getkey
		case 'g':
			//Get current key
			UART_Send(UARTx, (uint8_t *)&currentKey, sizeof(currentKey), BLOCKING);
			blinkBlueLed(100);
			cmd[0]=0;
			cmd[1]=0;
			break;
		case 's':
			//Set a new key
			currentKey = cmd[1];
			UART_SendByte(UARTx, readyToLoad);
			blinkBlueLed(100);
			cmd[0]=0;
			cmd[1]=0;
			break;
		case 'o':
			//Turn on radio
			TIM_ResetCounter(LPC_TIM0); //TMR0 enciende el adc, donde ocurre la transmision
			TIM_Cmd(LPC_TIM0, ENABLE);
			cmd[0]=0;
			cmd[1]=0;
		case 'f':
			//Turn off radio
			TIM_Cmd(LPC_TIM0, DISABLE);
			cmd[0]=0;
			cmd[1]=0;
			break;
		}
		//LPC_UART2 -> FCR |= (3 << 1);
	}
	return 0 ;
}

/*
 * Config UART
 * * BaudRate= ~115200(114882) (si PCLK=25MHz, lo es por defecto)
 * * P0.2 TX0
 * * P0.3 RX0
 */
void initUARTx(void)
{
	//p0.10 y p0.11 to UART2, pull-up
	PINSEL_CFG_Type cfgTXRX={
		.Funcnum=1,
		.Pinnum=10,
		.Portnum=0
	};
	PINSEL_ConfigPin(&cfgTXRX);

	cfgTXRX.Pinnum=11;
	PINSEL_ConfigPin(&cfgTXRX);

	//UART 115200 bauds, 8 bits data, 1 stop bit, no parity
	LPC_SC->PCONP |= 1<<24; //enciendo UART2 (por default encendido)

	LPC_UART2->LCR = 3|DLAB_BIT ; //8 bits, sin paridad, 1 Stop bit y DLAB enable
	//set baud rate
	LPC_UART2->DLL = 12;
	LPC_UART2->DLM = 0;
	LPC_UART2->FDR = (MULVAL<<4)|DIVADDVAL; //MULVAL=15(bits - 7:4), DIVADDVAL=2(bits - 3:0)

	/*
	 * Block divisor latches
	 */
	LPC_UART2->LCR &= ~(DLAB_BIT); //desactivando DLAB lockeamos los divisores para el Baudrate

	LPC_UART2->FCR |= Ux_FIFO_EN|Rx_FIFO_RST|Tx_FIFO_RST;
}
void initUART2(void)
{
	/*
	 * BaudRate= ~115200(114882) (si PCLK=25MHz, lo es por defecto)
	 */
	//p0.10 y p0.11 to UART2, pull-up
	PINSEL_CFG_Type cfgTXRX={
		.Funcnum=1,
		.Pinnum=10,
		.Portnum=0
	};
	PINSEL_ConfigPin(&cfgTXRX);

	cfgTXRX.Pinnum=11;
	PINSEL_ConfigPin(&cfgTXRX);

	LPC_SC->PCONP |= 1<<24; //enciendo UART2 (por default encendido)
	LPC_UART2->LCR = 3|DLAB_BIT ; //8 bits, sin paridad, 1 Stop bit y DLAB enable
	LPC_UART2->DLL = 12;
	LPC_UART2->DLM = 0;
	LPC_UART2->FDR = (MULVAL<<4)|DIVADDVAL; //MULVAL=15(bits - 7:4), DIVADDVAL=2(bits - 3:0)
	//bloquear divisor latches
	LPC_UART2->LCR &= ~(DLAB_BIT); //desactivando DLAB lockeamos los divisores para el Baudrate

	LPC_UART2->IER |= Rx_RDA_INT; //int cada vez que se reciban cierto numero de char (1 en este caso)
	LPC_UART2->FCR	= 0;
	LPC_UART2->FCR |= Ux_FIFO_EN|Rx_FIFO_RST|Tx_FIFO_RST;
	NVIC_ClearPendingIRQ(UART2_IRQn);
	NVIC_EnableIRQ(UART2_IRQn);
    NVIC_SetPriority(UART2_IRQn, 1); //cambiar
}



/*
 * Config ADC
 * * P0.23 to AD0.0
 * * pull-off
 */
void initADC(void) {
	PINSEL_CFG_Type ADC_pin;
	ADC_pin.Pinnum = 23;
	ADC_pin.Portnum = 0;
	ADC_pin.Funcnum = 1;
	ADC_pin.Pinmode = 2; //TRISTATE

	PINSEL_ConfigPin(&ADC_pin);

	//conf del ADC
	LPC_SC-> PCLKSEL0 |= (3 << 24);  // CCLK/8 = 100Mhz/8 = 12.5 MHz
	ADC_Init(LPC_ADC, 192307); //enciende PCONP, da PDN = 1 y carga un CLKDIV medio choto
	ADC_ChannelCmd(LPC_ADC, 0, ENABLE); //selecciona bits SEL, o sea el canal habiliado, recordar que es modo hardware
	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01); //configura START de ADCR
	ADC_EdgeStartConfig(LPC_ADC, ADC_START_ON_FALLING); //configura EDGE de ADCR
	ADC_IntConfig(LPC_ADC, ADC_ADGINTEN, SET); //configura individualmente las int por canales o la global
	NVIC_ClearPendingIRQ(ADC_IRQn);
	NVIC_EnableIRQ(ADC_IRQn);

	//AD0.1-AD0.7 a GPIO, pull-down, salida digital, estado bajo
	//tener en cuenta: AD0.3 DACOUT, AD0.6 y AD0.7 UART0
	unsigned char nullCh[6]={2,3,24,25,30,31};

	PINSEL_CFG_Type cfgNullCh={
		.Funcnum=0,
		.OpenDrain=PINSEL_PINMODE_NORMAL,
		.Pinmode=PINSEL_PINMODE_PULLDOWN,
	};

	for(unsigned char ch=0;ch<sizeof(nullCh);ch++){
		//ch del puerto 0
		if(ch<4){
			//pull-down, pin como salida
			cfgNullCh.Portnum=0;
			cfgNullCh.Pinnum=*(nullCh+ch);
			PINSEL_ConfigPin(&cfgNullCh);
			GPIO_SetDir(0, 1<<(*(nullCh+ch)), 1);
			//estado bajo
			GPIO_ClearValue(0, 1<<(*(nullCh+ch)));
		}

		//ch del puerto 1
		else{
			//pull-down, pin como salida
			cfgNullCh.Portnum=1;
			cfgNullCh.Pinnum=*(nullCh+ch);
			PINSEL_ConfigPin(&cfgNullCh);
			GPIO_SetDir(1, 1<<(*(nullCh+ch)), 1);
			//estado bajo
			GPIO_ClearValue(1, 1<<(*(nullCh+ch)));
		}
	}
}

/*
 * Config TMR0
 * * Recordar F_MRx = 2*prom*F_s(deseada)
 * * F_s = 3 KHz, prom = 8 -> F_MRx = 48 KHz -> 20.8us
 * * Con 12us -> F_s(real)= 47.619 KHz
 */
void initTMR0(void) {
	LPC_SC -> PCONP |= (1 << 1); //enciende TMR0 (recordar que ya lo esta por defecto);
	//LPC_SC -> PCLKSEL0 |= (1 << 2); //CCLK/1
	LPC_TIM0 -> EMR |= (3 << 6); //toglear (funcion 3) EM0 (MAT0.1)
	LPC_TIM0 -> PR = 25-1; //si PCLK = 25MHz entonces con ese PR se logra resolución de 1us
	LPC_TIM0 -> MR1 = 21-1;
	LPC_TIM0 -> MCR |= (2 << 3); //solo resetar cuando se llegue match, no int, no stop
	//LPC_TIM0 -> TCR = 3; //enable y reset en 1
	//LPC_TIM0 -> TCR &= ~(2); //quito el reset
}

void UART2_IRQHandler(void) {
	uint32_t uartIntSt = UART_GetIntId(LPC_UART2); //valor del registro IIR

	if (((uartIntSt & MASK_INT_ID) >> 1) == MASK_RDA_INT) {
		UART_Receive(LPC_UART2, cmd, sizeof(cmd), BLOCKING); // TODO: cmd de 1 byte?
	}
	//en teoria se limpia el flag cuando se efectua la lectura de los datos
}

void ADC_IRQHandler(){
	//incrementar contador de muestras
	samp++;
	//leer valor del adc
	res0=ADC_ChannelGetData(LPC_ADC, 0);
	//acumular valor de conversion y bajar banderas
	acc0+=res0;
	//promediar cada n muestras
	if(samp==n){
		acc0>>=(n/2)-1; // acc0/n
		//armar paquete
		uint8_t msb=(uint8_t)((acc0>>8)&0xFF);
		uint8_t lsb=(uint8_t)(acc0&0xFF);
		uint8_t pckt[]={currentKey,msb,lsb};
		//enviar a nRF
		nrf24_transmit_payload(pckt,sizeof(pckt));
		//reiniciar variables
		acc0=0;
		samp=0;
	}

	return;
}

void blinkRedLed(int time) {
	LED_RED_TOGGLE();
	delay_ms(time);
	LED_RED_TOGGLE();
}

void blinkGreenLed(int time) {
	LED_GREEN_TOGGLE();
	delay_ms(time);
	LED_GREEN_TOGGLE();
}

void blinkBlueLed(int time) {
	LED_BLUE_TOGGLE();
	delay_ms(time);
	LED_BLUE_TOGGLE();
}

void EINT0_IRQHandler(void) {
	uint8_t aux0[1];
	EXTI_ClearEXTIFlag(0);
	uint8_t status= nrf24_status();
	if(status & RF24_MASK_MAX_RT){
		//error de retransmision
		aux0[0] = status | RF24_MASK_MAX_RT;
		nrf24_writeToNrf(W, RF24_STATUS, aux0 , sizeof(aux0)); //limpio el flag MAX_RT (maximo de retrasmisiones)
		count_error++;
		if(count_error > COUNT_ERR_TRG) {
			FlagErrorTransmission = 1;
			count_error = 0;
		}

	}
	else if(status & (RF24_MASK_TX_DS|RF24_MASK_RX_DR)){
		//transmision exitosa
		aux0[0] = status | RF24_MASK_TX_DS | RF24_MASK_RX_DR;
		nrf24_writeToNrf(W, RF24_STATUS, aux0 , sizeof(aux0)); //limpio el flag TX_DS y RX_DR
		blinkGreenLed(10);
	}
}
