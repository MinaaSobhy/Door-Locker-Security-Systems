/*
 * buzzer.c
 *
 *  Created on: Oct 29, 2022
 *      Author: Mina Sobhy
 *      description: source file for buzzer
 */
#include "buzzer.h"


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description : Function to initialize the buzzer driver
 * 	1. Set the buzzer pin as an output pin
 * 	2. Turn off the buzzer
 */
void Buzzer_init(void)
{
	/*set the buzzer pin as an output pin */
	GPIO_setupPinDirection(BUZZER_PORT,BUZZER_PIN, PIN_OUTPUT);

	/* initially, Turn off the buzzer */
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN, LOGIC_LOW);
}

/*
 * Description: Function to enable the buzzer
 */
void Buzzer_on(void)
{
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN, LOGIC_HIGH);
}

/*
 * Description: Function to disable the buzzer
 */
void Buzzer_off(void)
{
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN, LOGIC_LOW);
}
