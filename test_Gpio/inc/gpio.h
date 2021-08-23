#include "LPC17xx.h"
/**--------------------------------------------------------------------------------
 * Configuracion GPIO0
 * @param pin numero del pin a configurar
 * @param modo modo de configuracion
 *--------------------------------------------------------------------------------
 * */
void setConfGpio0(LPC_GPIO_TypeDef * gpio,uint8_t pin,uint8_t modo,uint8_t dir);


void setConfGpio1(LPC_GPIO_TypeDef * gpio,uint8_t pin,uint8_t modo,uint8_t dir);


void setConfGpio2(LPC_GPIO_TypeDef * gpio,uint8_t pin,uint8_t modo,uint8_t dir);


void setConfGpio3(LPC_GPIO_TypeDef * gpio,uint8_t pin,uint8_t modo,uint8_t dir);


void setConfGpio4(LPC_GPIO_TypeDef * gpio,uint8_t pin,uint8_t modo,uint8_t dir);

void setValueGpio(LPC_GPIO_TypeDef * gpio,uint8_t pin,uint8_t value);


uint32_t getValueGpio(LPC_GPIO_TypeDef * gpio);
