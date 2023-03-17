/*
 * timer1.h
 *
 *  Created on: Oct 27, 2022
 *      Author: Mina sobhy
 *      description: header file for timer1
 */

#ifndef TIMER1_H_
#define TIMER1_H_

#include "std_types.h"


/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
  NOCLOCK, PRESCALER_1, PRESCALER_8, PRESCALER_64, PRESCALER_256, PRESCALER_1024
} Timer1_Prescaler ;

typedef enum
{
  NORMAL_MODE=1, COMPARE_MODE=4
} Timer1_Mode;

typedef struct {
	uint16 				initial_value	;
	uint16 				compare_value	; // it will be used in compare mode only.
	Timer1_Prescaler 	prescaler		;
	Timer1_Mode			mode			;
} Timer1_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description : Function to initialize the ICU driver
 * 	1. Set initial value for timer1
 * 	2. Set the compare value
 * 	3. Enable the overflow or compare A Interrupt.
 * 	4. configure timer 1 registers (TCCR1A, TCCR1B)
 * 	5. Set the required clock
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr);

/*
 * Description: Function to disable the Timer1
 */
void Timer1_deInit(void);

/*
 * Description: Function to set the Call Back function address.
 */
void Timer1_setCallBack(void(*a_ptr)(void));


#endif /* TIMER1_H_ */
