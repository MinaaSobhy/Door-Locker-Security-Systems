/*
 * dcmotor.c
 *
 *  Created on: Oct 6, 2022
 *      Author: Mina sobhy
 *      description: function to initialize DC Motor and rotate it
 */

#include "dcmotor.h"
#include "gpio.h"
#include "pwm_timer0.h"

/* setting output pins for the motor and set an initializing value for them */
void DcMotor_init(void)
{
	/* configure pins (PB4 and PB5) as output pins */
	GPIO_setupPinDirection(DCmotor_PORTA,DCmotor_PINA, PIN_OUTPUT);
	GPIO_setupPinDirection(DCmotor_PORTB,DCmotor_PINB, PIN_OUTPUT);


	/* Clear the two bits to stop the motor at the beginning */
	GPIO_writePin(DCmotor_PORTA,DCmotor_PINA, LOGIC_LOW);
	GPIO_writePin(DCmotor_PORTB,DCmotor_PINB, LOGIC_LOW);
}

/* setting the speed of the motor according to the input */
void DcMotor_Rotate(DcMotor_State a_state, uint8 a_speed)
{

	/* rotate the motor according to the current state
	 * if state != 0 or != 1 or != 2 then do nothing
	 */
	switch(a_state)
	{
	case 0:
		GPIO_writePin(DCmotor_PORTA,DCmotor_PINA, LOGIC_LOW);
		GPIO_writePin(DCmotor_PORTB,DCmotor_PINB, LOGIC_LOW);
		break;
	case 1:
		GPIO_writePin(DCmotor_PORTA,DCmotor_PINA, LOGIC_LOW);
		GPIO_writePin(DCmotor_PORTB,DCmotor_PINB, LOGIC_HIGH);
		break;
	case 2:
		GPIO_writePin(DCmotor_PORTA,DCmotor_PINA, LOGIC_HIGH);
		GPIO_writePin(DCmotor_PORTB,DCmotor_PINB, LOGIC_LOW);
		break;
	}

	if (a_speed < 0 || a_speed > 100 )
	{
		/* incorrect speed so do nothing */
	}
	else
	{
		/* convert the speed form percentage to bits according to the register size(256)
		 * and calling PWM function to generate the speed required by the motor */
		PWM_Timer0_Start((uint8)((uint32)(a_speed*255)/100));
	}
}
