/*
 * timers.h
 *
 *  Created on: 31 oct. 2021
 *      Author: PC
 */

#ifndef TIMERS_H_
#define TIMERS_H_



#endif /* TIMERS_H_ */

#define PRESCALER_VALUE_TM1   1
#define PRESCALER_VALUE_TM2   1000
void confTimer1 (void);
void confTimer2 (void);
void delay_us (uint16_t time);
void delay_ms(uint16_t time);
