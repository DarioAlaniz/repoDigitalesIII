/*
===============================================================================
Generar con timer2 una señal de freq. variable. P0.9, probar sin match.
Usando el capture “medir” el periodo usando otro timer3. cap P0.23
Prender leds tipo vúmetro según la frecuencia.
Con un pulsador cambiar la frecuencia de pasos de 100khz. Actualizar el vúmetro.
===============================================================================
*/

/*
 * Si aumento de 100khz y dejando fijo una base de tiempo de 2e-8 a medida que disminuyo el valor del match
 * aumenta la frecuencia entonces para establer 100khz --> 10us --> match value = 999
 * 												200khz --> 5us  --> match value = 499
 * 												300khz --> 3.33us --> match value = 332
 * 												.			. 						.
 * 												.			.						.
 * 												n*100khz --> 10us/n  --> match value = (1000/n) -1
 *
 * */
#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif
#include <cr_section_macros.h>

#include "lpc17xx_timer.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_exti.h"


#define LIMIT_FREQ_MED 	4199998   	//valor sacado para 42 seg con una base de tiempo de 100
#define OFFSET_OVERFLOW 0XFFFFFFFF
#define INITIAL_VALUE	500			//valor inicial para establer una frecuencia de 100khz del timer2

void confGpio(void);
void confIntExt(void);
void confTimer2(void);
void confTimer3(void);
void resetTimers(void);


uint8_t flag_Eint0		= 0;
uint8_t flag_CapTimer3 	= 0;
uint8_t firt_int_timer3 = 0;
uint32_t current_val;
uint32_t previous_val;
uint32_t period;
uint8_t count			= 1;
int main(void) {
	confGpio();
	confIntExt();
	confTimer2();
	confTimer3();
	resetTimers();
    while(1) {
    	if(flag_Eint0){
    		flag_Eint0 		= 0;

    		if(count > 10) count = 1; //limite de frecuencia del timer2 lo establesco en 1Mhz

    		TIM_Cmd(LPC_TIM2, 0);   //paro el timer2 para evitar que justo toggle P0.9 antes de actualizar el valor de match
    		LPC_TIM2->MR3 	=  (INITIAL_VALUE/count) - 1;; 	//disminuyo el matchvalue del timer2, asi aumentar la frecuencia
    								//como el timer2 esta seteado con una base de tiempo de 10us -->100Khz
    								//entonces aumentar el match hace que duplique esa frecuencia
    		TIM_Cmd(LPC_TIM2, 1);   //vuelvo a habilitar el timer2
    		TIM_ResetCounter(LPC_TIM2);
    	}
    	if(flag_CapTimer3){
    		flag_CapTimer3 	= 0;
    		if(!firt_int_timer3){
    			firt_int_timer3 = 1; 			// no hago nada en la primera vez que entra
    		}
    		else if(current_val<previous_val){  // si se cumple quiere decir que justo el timer3 se reseteo
    			period = OFFSET_OVERFLOW + 	current_val - previous_val; //sumo el offset del reset
    		}
    		else{
    			period = current_val - previous_val;	//obtengo el periodo
    			//para obtener el periodo hacemos (period + 1)/100e6
    			//no se cumple cuando el periodo de la señal es igual o mayor a 500khz
    			//puede ser un problema del tiempo de respuesta del pin, consultar!!!!
    		}
    		previous_val 	= current_val;  	//copio el valor capturado
    	}
    }
    return 0 ;
}


void confGpio(void){
	//configuro un pin de match, otro de capture y por ultimo de interrupcion externa
	PINSEL_CFG_Type pinCfg;
	pinCfg.Portnum 	= 0;
	pinCfg.Pinnum 	= 9; 	//P0.9
	pinCfg.Funcnum 	= 3; 	//MAT2.3
	PINSEL_ConfigPin(&pinCfg);
	pinCfg.Pinnum 	= 23; 	//P0.23
	pinCfg.Funcnum 	= 3; 	//CAP3.0
	pinCfg.Pinmode	= 1;	//sin pull-down ni pull-up
	PINSEL_ConfigPin(&pinCfg);
	pinCfg.Portnum 	= 2;
	pinCfg.Pinnum	= 10;  	//P2.10
	pinCfg.Funcnum	= 1;	//EINT0
	pinCfg.Pinmode	= 0;	//pull-up
	PINSEL_ConfigPin(&pinCfg);
	return;
}

void confIntExt(void){
	EXTI_InitTypeDef Eint0Cfg;
	Eint0Cfg.EXTI_Line 		= 0; 		//EINT0
	Eint0Cfg.EXTI_Mode		= 1;		//activo por flanco
	Eint0Cfg.EXTI_polarity	= 0;   		//flanco de bajada
	EXTI_Config(&Eint0Cfg);				//configuro la interrupcion, limpia la bandera
	NVIC_SetPriority(EINT0_IRQn, 5);	//establesco menor prioridad que el timer3
	NVIC_EnableIRQ(EINT0_IRQn); 		//habilito la interrupcion
	return;
}

void confTimer3(void){
	TIM_TIMERCFG_Type 	prescalerTmr3Cfg;
	TIM_CAPTURECFG_Type capTmr3Cfg;
	prescalerTmr3Cfg.PrescaleOption		= 0;
	prescalerTmr3Cfg.PrescaleValue		= 1; //maximo valor que a 100Mhz da un periodo minimo de 10nSeg --> maxima frecuencia a medir: 100Mhz
	//conf mode capture
	capTmr3Cfg.CaptureChannel			= 0;  //canal 0
	capTmr3Cfg.RisingEdge				= 1;  //por flanco de subida
	capTmr3Cfg.IntOnCaption				= 1;  //Habilito las interrupciones

	TIM_Init(LPC_TIM3, TIM_TIMER_MODE, &prescalerTmr3Cfg);
	//Cambio el divisor de clock de perifericos al timer0 con divisor por 1(defecto viene por 4 = 25Mhz)
	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER3,CLKPWR_PCLKSEL_CCLK_DIV_1); //100Mhz

	TIM_ConfigCapture(LPC_TIM3, &capTmr3Cfg);
	TIM_ClearIntCapturePending(LPC_TIM3,4);	  	//limpio la bandera del CAP3.0
	NVIC_SetPriority(TIMER3_IRQn, 4);			//mayor prioridad que EINT0
	NVIC_EnableIRQ(TIMER3_IRQn); 			  	//habilito la interrupcion del timer 3
	TIM_Cmd(LPC_TIM3,1);					  	//habilito el timer3
	return;
}

void confTimer2(void){
	TIM_MATCHCFG_Type	matchTmr2Cfg;
	TIM_TIMERCFG_Type 	prescalerTmr2Cfg;
	prescalerTmr2Cfg.PrescaleOption 	= 0;
	prescalerTmr2Cfg.PrescaleValue 		= 1; 	//maxima resolucion con prescaler en 1 es 2e-8 --> 50Mhz
	//conf match mode
	matchTmr2Cfg.MatchChannel			= 3; 	//MAT2.3
	matchTmr2Cfg.ExtMatchOutputType		= 3;    //toggle P0.9
	matchTmr2Cfg.ResetOnMatch			= 1;
	matchTmr2Cfg.StopOnMatch			= 0;
	matchTmr2Cfg.MatchValue				= (INITIAL_VALUE/count) - 1;	//valor a cambiar por medio de EINT0 para aumentar cada 100Khz
	matchTmr2Cfg.IntOnMatch				= 0;

	TIM_Init(LPC_TIM2, TIM_TIMER_MODE, &prescalerTmr2Cfg);
	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER2, CLKPWR_PCLKSEL_CCLK_DIV_1);
	TIM_ConfigMatch(LPC_TIM2, &matchTmr2Cfg);
	TIM_Cmd(LPC_TIM2, 1);						//habilito el timer
	return ;
}

void resetTimers(void){
	//reseto los timer para sincronizar
	TIM_ResetCounter(LPC_TIM2);
	TIM_ResetCounter(LPC_TIM3);
	return;
}

void EINT0_IRQHandler(void){
	EXTI_ClearEXTIFlag(0); 				//limpio el flag
	flag_Eint0 	= 1;					//seteo el flag
	count 		+=1;
}

void TIMER3_IRQHandler(void){
	current_val		= TIM_GetCaptureValue(LPC_TIM3, 0); //guardo el valor del timer3
	TIM_ClearIntCapturePending(LPC_TIM3,4);	  			//limpio la bandera del CAP3.0
	flag_CapTimer3 = 1;
}


/*
 * chequear interrup del cap se traba en algunos casos
 * no funciona a lo esperado*/
