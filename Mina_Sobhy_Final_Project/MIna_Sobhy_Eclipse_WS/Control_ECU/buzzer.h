/*
 * buzzer.h
 *
 *  Created on: Oct 29, 2022
 *      Author: mina sobhy
 *      description: header file for buzzer
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"
#include "gpio.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
//Define Buzzer port
#define BUZZER_PORT PORTA_ID
//Define Buzzer pin
#define BUZZER_PIN PIN0_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description : Function to initialize the buzzer driver
 * 	1. Set the buzzer pin as an output pin
 * 	2. Turn off the buzzer
 */
void Buzzer_init(void);

/*
 * Description: Function to enable the buzzer
 */
void Buzzer_on(void);

/*
 * Description: Function to disable the buzzer
 */
void Buzzer_off(void);


#endif /* BUZZER_H_ */
