/*
 * timer1.c
 *
 *  Created on: Oct 27, 2022
 *      Author: Mina sobhy
 *      description: source file for timer1
 */

#include "timer1.h"
#include <avr/interrupt.h>
#include <avr/io.h>

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/
ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the OVF ISR is fired */
		(*g_callBackPtr)();
	}
}

ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the COMPA ISR is fired */
		(*g_callBackPtr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description : Function to initialize the ICU driver
 * 	1. Set initial value for timer1
 * 	2. Set the compare value
 * 	3. Enable the overflow or compare A Interrupt.
 * 	4. configure timer 1 registers (TCCR1A, TCCR1B)
 * 	5. Set the required clock
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr)
{
	/* Set timer1 initial count to the initial configuration */
	TCNT1 = Config_Ptr->initial_value;

	/* Set timer1 compare value according to the configuration */
	OCR1A = Config_Ptr->compare_value;

	/* Enable Timer1 Compare A Interrupt or Overflow Interrupt depend on mode configuration*/
	TIMSK = (TIMSK & 0xEB) | ( ((Config_Ptr->mode)<<2) & 0x14);

	/* Configure timer control register TCCR1A
	 * 1. Disconnect OC1A and OC1B  COM1A1=0 COM1A0=0 COM1B0=0 COM1B1=0
	 * 2. FOC1A=1 FOC1B=1 (Non-PWM)
	 * 3. CTC Mode WGM10=0 WGM11=0 (Mode Number 4 or Mode Number 0)
	 */
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);

	/* Configure timer control register TCCR1B
	 * 1. Mode configured to be mode number 0(Normal) or mode number 4 (CTC)
	 * 2. Pre-scaler = depend on the pre-scaler configuration
	 */
	TCCR1B = (TCCR1B & 0xF7) | ( ((Config_Ptr->mode)<<1) & 0x08);
	TCCR1B = (TCCR1B & 0xF8) | ( (Config_Ptr->prescaler) & 0x07);
}

/*
 * Description: Function to disable the Timer1
 */
void Timer1_deInit(void)
{
	/* Clear all timer registers */
	TCCR1A	= 0;
	TCCR1B	= 0;
	TCNT1	= 0;
	OCR1A	= 0;

	/* disable Timer1 Compare A Interrupt or Overflow Interrupt depend on mode configuration */
	TIMSK &= 0xCF;
}

/*
 * Description: Function to set the Call Back function address.
 */


void Timer1_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}






