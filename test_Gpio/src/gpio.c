//#include "LPC17xx.h"
#include "gpio.h"
#define LIMIT 16U
#define MASK 0xFF

/*
 * Cambiar el valor con el que configura el pin para el modo y pinsel(seguir esquema de setConfGpio0)
 * setConfGpio0 esta parcial mente cambiado hay que cambiar que no guarde el valor del pin
 * en el mismo parametro de entrada sino en otro ya que lo uso para fiodir,
 *  faltar arreglar para las demas funciones!!!!
 * */


/**--------------------------------------------------------------------------------
 * Configuracion GPIO
 * @param uint8_t 	numero de gpio a configurar.
 * @param pin 		numero del pin a configurar.
 * @param modo 		modo de configuracion.
 * 					0 pin has a pull-up resistor enabled.
 * 					1 pin has repeater mode enabled.
 * 					2 pin has neither pull-up nor pull-down.
 * 					3 has a pull-down resistor enabled.
 * @param dir 		0-->input/ 1-->output.
 *
 * Nota :  no funciona para todos los gpio, solo para gpio 0,1
 *--------------------------------------------------------------------------------
 * */

void setConfGpio(uint8_t gpio,uint8_t pin,uint8_t modo,uint8_t dir){
	uint8_t pin_aux = pin;
	int * pPinSel 		= (int *)(LPC_PINCON_BASE+(sizeof(int)*gpio*2));		//PINSEL0 ---> 0x4002 C000
	int * pPinMode 		= (int *)(LPC_PINCON_BASE + (sizeof(int) * 16)+ (sizeof(int)*gpio*2)); 	//PINMODE0 ---> 0x4002 C040
	int * gpioFioDir 	= (int* )(LPC_GPIO_BASE + (sizeof(int)*8*gpio));				//FIODIR ----> 0x2009 C000
	if (pin<LIMIT){
		pin_aux 		= (MASK&pin*2);
		*(pPinSel)		&= ~(0b11<< pin_aux);
		switch(0x03&modo){
			case 0:// 00 pin has a pull-up resistor enabled.
				*pPinMode 	&= ~(0b11<< pin_aux);
				break;
			case 1:// 01 pin has repeater mode enabled.
				*pPinMode 	|= (0b1<< pin_aux);
				*pPinMode 	&= ~(0b1<< (pin_aux+1));
				break;
			case 2: //10 pin has neither pull-up nor pull-down.
				*pPinMode 	&= ~(0b1<< pin_aux);
				*pPinMode 	|= (0b1<< (pin_aux+1));
				break;
			case 3: //11 has a pull-down resistor enabled.
				*pPinMode 	|= (0b11<< pin_aux);
				break;
			}
	}
	else{
		pin_aux 		= ((MASK&pin*2)-32);
		*(pPinSel + 1)	&= ~(0b11<< pin_aux);
		switch(0x03&modo){
			case 0:// 00 pin has a pull-up resistor enabled.
				*(pPinMode+1) 	&= ~(0b11<< pin_aux);
				break;
			case 1:// 01 pin has repeater mode enabled.
				*(pPinMode+1) 	|= (0b1<< pin_aux);
				*(pPinMode+1) 	&= ~(0b1<< (pin_aux+1));
				break;
			case 2: //10 pin has neither pull-up nor pull-down.
				*(pPinMode+1) 	&= ~(0b1<< pin_aux);
				*(pPinMode+1) 	|= (0b1<< (pin_aux+1));
				break;
			case 3: //11 has a pull-down resistor enabled.
				*(pPinMode+1) 	|= (0b11<< pin_aux);
				break;
			}
	}
	//Configuro la direccion si es entrada 0, salida 1
	if( (0x1&dir) == 0b1)
		*gpioFioDir |= ((0x1&dir)<<pin); //output
	else
		*gpioFioDir &= ~((0x1&dir)<<pin); //input
}

/**--------------------------------------------------------------------------------
 * Configuracion Setea un valor en el pin del gpio dado
 * @param *gpio puntero a la estructura del gpio
 * @param pin 	numero del pin a configurar
 * @param value valor a cargar 1 o 0
 *--------------------------------------------------------------------------------
 * */
void setValueGpio(LPC_GPIO_TypeDef * gpio,uint8_t pin,uint8_t value){
	if(value == 1)
		gpio->FIOSET  |= (1<<(0x3F&pin));
	else
		gpio->FIOCLR  |= (1<<(0x3F&pin));
}

/**--------------------------------------------------------------------------------
 * Obtiene el valor de todo el registro del gpio
 * @param *gpio puntero a la estructura del gpio
 *
 * @return Contenido de todo el registro Gpio
 *--------------------------------------------------------------------------------
 * */
uint32_t getValueGpio(LPC_GPIO_TypeDef * gpio){
	return gpio->FIOPIN;
}

