/*
 * dcmotor.h
 *
 *  Created on: Oct 6, 2022
 *      Author: Mina sobhy
 *      description: header file to declare DC Motor functions prototypes
 */

#ifndef DCMOTOR_H_
#define DCMOTOR_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

//Define ports
#define DCmotor_PORTA PORTB_ID
#define DCmotor_PORTB PORTB_ID

//Define pins
#define DCmotor_PINA PIN1_ID
#define DCmotor_PINB PIN2_ID

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
	STOP, ACW , CW
} DcMotor_State ;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Initialize the DCmotor:
 * 1. Setup the DCmotor pins directions by use the GPIO driver.
 * 2. Setup the DCmotor pins to be initially LOW to stop the motor.
 */
void DcMotor_init(void);

/*
 * Description :
 * 1. Setup the Direction of the motion of the DCmotor.
 * 2. Setup the speed of the motor by calculating number of bits and send it to PWM Function.
 */

void DcMotor_Rotate(DcMotor_State a_state, uint8 a_speed);


#endif /* DCMOTOR_H_ */
