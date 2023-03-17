/*
 * pwm_timer0.h
 *
 *  Created on: Oct 6, 2022
 *      Author: Mina sobhy
 *      description: Header file for PWM driver made on timer0
 */

#ifndef PWM_TIMER0_H_
#define PWM_TIMER0_H_

#include "std_types.h"
/*******************************************************************************
 *                              Functions Prototypes                           *
 *******************************************************************************/

/* Description :
 *1. The function responsible for trigger the Timer0 with the PWM Mode.
 *2. Setup the PWM mode with Non-Inverting.
 *3. Setup the prescaler with F_CPU/8.
 *4. Setup the compare value based on the required input duty cycle
 *5. Setup the direction for OC0 as output pin through the GPIO driver.
 *6. The generated PWM signal frequency will be 500Hz to control the DC Motor speed.
 */

void PWM_Timer0_Start(uint8 a_dutyCycle);

#endif /* PWM_TIMER0_H_ */
