//#include "LPC17xx.h"
#include "gpio.h"
#define limit 16U
#define mask 0xFF

/*
 * Cambiar el valor con el que configura el pin para el modo y pinsel(seguir esquema de setConfGpio0)
 * setConfGpio0 esta parcial mente cambiado hay que cambiar que no guarde el valor del pin
 * en el mismo parametro de entrada sino en otro ya que lo uso para fiodir,
 *  faltar arreglar para las demas funciones!!!!
 * */


/**--------------------------------------------------------------------------------
 * Configuracion GPIO0
 * @param *gpio puntero a la estructura del gpio
 * @param pin 	numero del pin a configurar
 * @param modo 	modo de configuracion
 * 				0 pin has a pull-up resistor enabled.
 * 				1 pin has repeater mode enabled.
 * 				2 pin has neither pull-up nor pull-down.
 * 				3 has a pull-down resistor enabled.
 * @param dir 	0-->input/ 1-->output
 *--------------------------------------------------------------------------------
 * */
void setConfGpio0(LPC_GPIO_TypeDef * gpio,uint8_t pin,uint8_t modo,uint8_t dir){
	if(pin<limit) {
		pin = (mask&pin*2)
		LPC_PINCON->PINSEL0  &= ~(0b11<< pin);
		switch(0x03&modo){
		case 0:// 00 pin has a pull-up resistor enabled.
			LPC_PINCON->PINMODE0 &= ~(0b11<< pin);
			break;
		case 1:// 01 pin has repeater mode enabled.
			LPC_PINCON->PINMODE0 |= (0b1<< pin);
			LPC_PINCON->PINMODE0 &= ~(0b1<< (pin+1));
			break;
		case 2: //10 pin has neither pull-up nor pull-down.
			LPC_PINCON->PINMODE0 &= ~(0b1<< pin);
			LPC_PINCON->PINMODE0 |= (0b1<< (pin+1));
			break;
		case 3: //11 has a pull-down resistor enabled.
			LPC_PINCON->PINMODE0 |= (0b11<< pin);
			break;
		}
	}
	else {
		pin = ((mask&pin*2)-32)
		LPC_PINCON->PINSEL1 &= ~(0b11<<	pin);
		switch(0x03&modo){
		case 0:// 00 pin has a pull-up resistor enabled.
			LPC_PINCON->PINMODE1 &= ~(0b11<< pin);
			break;
		case 1:// 01 pin has repeater mode enabled.
			LPC_PINCON->PINMODE1 |= (0b1<< pin);
			LPC_PINCON->PINMODE1 &= ~(0b1<< (pin+1));
			break;
		case 2: //10 pin has neither pull-up nor pull-down.
			LPC_PINCON->PINMODE1 &= ~(0b1<< pin);
			LPC_PINCON->PINMODE1 |= (0b1<< (pin+1));
			break;
		case 3: //11 has a pull-down resistor enabled.
			LPC_PINCON->PINMODE1 |= (0b11<< pin);
			break;
		}
	}
	//Configuro la direccion si es entrada 0, salida 1
	if( (0x1&dir) == 0b1)
		gpio->FIODIR |= (0x1&dir<<pin); //output
	else
		gpio->FIODIR &= ~(0x1&dir<<pin); //input
}

void setConfGpio1(LPC_GPIO_TypeDef * gpio,uint8_t pin,uint8_t modo,uint8_t dir){
		if(pin<=16) {
			LPC_PINCON->PINSEL2  &= ~(0b11<< (0xFF&pin*2));
			switch(0x03&modo){
			case 0:// 00 pin has a pull-up resistor enabled.
				LPC_PINCON->PINMODE2 &= ~(0b11<< (0xFF&pin*2));
				break;
			case 1:// 01 pin has repeater mode enabled.
				LPC_PINCON->PINMODE2 |= (0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE2 &= ~(0b1<< ((0xFF&pin*2)+1));
				break;
			case 2: //10 pin has neither pull-up nor pull-down.
				LPC_PINCON->PINMODE2 &= ~(0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE2 |= (0b1<< ((0xFF&pin*2)+1));
				break;
			case 3: //11 has a pull-down resistor enabled.
				LPC_PINCON->PINMODE2 |= (0b11<< (0xFF&pin*2));
				break;
			}
		}
		else {
			LPC_PINCON->PINSEL3 &= ~(0b11<<	((0xFF&pin*2)-32));
			switch(0x03&modo){
			case 0:// 00 pin has a pull-up resistor enabled.
				LPC_PINCON->PINMODE3 &= ~(0b11<< (0xFF&pin*2));
				break;
			case 1:// 01 pin has repeater mode enabled.
				LPC_PINCON->PINMODE3 |= (0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE3 &= ~(0b1<< ((0xFF&pin*2)+1));
				break;
			case 2: //10 pin has neither pull-up nor pull-down.
				LPC_PINCON->PINMODE3 &= ~(0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE3 |= (0b1<< ((0xFF&pin*2)+1));
				break;
			case 3: //11 has a pull-down resistor enabled.
				LPC_PINCON->PINMODE3 |= (0b11<< (0xFF&pin*2));
				break;
			}
		}
		//Configuro la direccion si es entrada 0, salida 1
		if( (0x1&dir) == 0b1) gpio->FIODIR |= (0x1&dir<<pin);
		else gpio->FIODIR &= ~(0x1&dir<<pin);

}

void setConfGpio2(LPC_GPIO_TypeDef * gpio,uint8_t pin,uint8_t modo,uint8_t dir){
		if(pin<=15) {
			LPC_PINCON->PINSEL4  &= ~(0b11<< (0xFF&pin*2));
			switch(0x03&modo){
			case 0:// 00 pin has a pull-up resistor enabled.
				LPC_PINCON->PINMODE4 &= ~(0b11<< (0xFF&pin*2));
				break;
			case 1:// 01 pin has repeater mode enabled.
				LPC_PINCON->PINMODE4 |= (0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE4 &= ~(0b1<< ((0xFF&pin*2)+1));
				break;
			case 2: //10 pin has neither pull-up nor pull-down.
				LPC_PINCON->PINMODE4 &= ~(0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE4 |= (0b1<< ((0xFF&pin*2)+1));
				break;
			case 3: //11 has a pull-down resistor enabled.
				LPC_PINCON->PINMODE4 |= (0b11<< (0xFF&pin*2));
				break;
			}
		}
		else {
			LPC_PINCON->PINSEL5 &= ~(0b11<<	((0xFF&pin*2)-32));
			switch(0x03&modo){
			case 0:// 00 pin has a pull-up resistor enabled.
				LPC_PINCON->PINMODE5 &= ~(0b11<< (0xFF&pin*2));
				break;
			case 1:// 01 pin has repeater mode enabled.
				LPC_PINCON->PINMODE5 |= (0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE5 &= ~(0b1<< ((0xFF&pin*2)+1));
				break;
			case 2: //10 pin has neither pull-up nor pull-down.
				LPC_PINCON->PINMODE5 &= ~(0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE5 |= (0b1<< ((0xFF&pin*2)+1));
				break;
			case 3: //11 has a pull-down resistor enabled.
				LPC_PINCON->PINMODE5 |= (0b11<< (0xFF&pin*2));
				break;
			}
		}
		//Configuro la direccion si es entrada 0, salida 1
		if( (0x1&dir) == 0b1) gpio->FIODIR |= (0x1&dir<<pin);
		else gpio->FIODIR &= ~(0x1&dir<<pin);
}

void setConfGpio3(LPC_GPIO_TypeDef * gpio,uint8_t pin,uint8_t modo,uint8_t dir){
		if(pin<=15) {
			LPC_PINCON->PINSEL6  &= ~(0b11<< (0xFF&pin*2));
			switch(0x03&modo){
			case 0:// 00 pin has a pull-up resistor enabled.
				LPC_PINCON->PINMODE6 &= ~(0b11<< (0xFF&pin*2));
				break;
			case 1:// 01 pin has repeater mode enabled.
				LPC_PINCON->PINMODE6 |= (0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE6 &= ~(0b1<< ((0xFF&pin*2)+1));
				break;
			case 2: //10 pin has neither pull-up nor pull-down.
				LPC_PINCON->PINMODE6 &= ~(0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE6 |= (0b1<< ((0xFF&pin*2)+1));
				break;
			case 3: //11 has a pull-down resistor enabled.
				LPC_PINCON->PINMODE6 |= (0b11<< (0xFF&pin*2));
				break;
			}
		}
		else {
			LPC_PINCON->PINSEL7 &= ~(0b11<<	((0xFF&pin*2)-32));
			switch(0x03&modo){
			case 0:// 00 pin has a pull-up resistor enabled.
				LPC_PINCON->PINMODE7 &= ~(0b11<< (0xFF&pin*2));
				break;
			case 1:// 01 pin has repeater mode enabled.
				LPC_PINCON->PINMODE7 |= (0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE7 &= ~(0b1<< ((0xFF&pin*2)+1));
				break;
			case 2: //10 pin has neither pull-up nor pull-down.
				LPC_PINCON->PINMODE7 &= ~(0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE7 |= (0b1<< ((0xFF&pin*2)+1));
				break;
			case 3: //11 has a pull-down resistor enabled.
				LPC_PINCON->PINMODE7 |= (0b11<< (0xFF&pin*2));
				break;
			}
		}
		//Configuro la direccion si es entrada 0, salida 1
		if( (0x1&dir) == 0b1) gpio->FIODIR |= (0x1&dir<<pin);
		else gpio->FIODIR &= ~(0x1&dir<<pin);
}

void setConfGpio4(LPC_GPIO_TypeDef * gpio,uint8_t pin,uint8_t modo,uint8_t dir){
		if(pin<=15) {
			LPC_PINCON->PINSEL8  &= ~(0b11<< (0xFF&pin*2));
			switch(0x03&modo){
			case 0:// 00 pin has a pull-up resistor enabled.
				LPC_PINCON->PINMODE8 &= ~(0b11<< (0xFF&pin*2));
				break;
			case 1:// 01 pin has repeater mode enabled.
				LPC_PINCON->PINMODE8 |= (0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE8 &= ~(0b1<< ((0xFF&pin*2)+1));
				break;
			case 2: //10 pin has neither pull-up nor pull-down.
				LPC_PINCON->PINMODE8 &= ~(0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE8 |= (0b1<< ((0xFF&pin*2)+1));
				break;
			case 3: //11 has a pull-down resistor enabled.
				LPC_PINCON->PINMODE8 |= (0b11<< (0xFF&pin*2));
				break;
			}
		}
		else {
			LPC_PINCON->PINSEL9 &= ~(0b11<<	((0xFF&pin*2)-32));
			switch(0x03&modo){
			case 0:// 00 pin has a pull-up resistor enabled.
				LPC_PINCON->PINMODE9 &= ~(0b11<< (0xFF&pin*2));
				break;
			case 1:// 01 pin has repeater mode enabled.
				LPC_PINCON->PINMODE9 |= (0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE9 &= ~(0b1<< ((0xFF&pin*2)+1));
				break;
			case 2: //10 pin has neither pull-up nor pull-down.
				LPC_PINCON->PINMODE9 &= ~(0b1<< (0xFF&pin*2));
				LPC_PINCON->PINMODE9 |= (0b1<< ((0xFF&pin*2)+1));
				break;
			case 3: //11 has a pull-down resistor enabled.
				LPC_PINCON->PINMODE9 |= (0b11<< (0xFF&pin*2));
				break;
			}
		}
		//Configuro la direccion si es entrada 0, salida 1
		if( (0x1&dir) == 0b1) gpio->FIODIR |= (0x1&dir<<pin);
		else gpio->FIODIR &= ~(0x1&dir<<pin);
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

