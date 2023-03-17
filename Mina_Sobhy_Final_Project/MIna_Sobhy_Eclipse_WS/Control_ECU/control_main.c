/*
 * control_main.c
 *
 *  Created on: Oct 29, 2022
 *      Author: Mina Sobhy
 *      Description: responsible for all the processing and decisions in the system like password
 *					 checking, open the door and activate the system alarm
 */


#include "dcmotor.h"
#include "uart.h"
#include "timer1.h"
#include "twi.h"
#include "external_eeprom.h"
#include "buzzer.h"
#include <util/delay.h>
#include <avr/io.h>


/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define MATCHED			'1'
#define UNMATCHED 		'0'
#define COMPARE_ERROR	'2'

/* as the CONTROL_ECU is faster than the HMI_ECU. So, we won't need a start bit
 * from the CONTOL_ECU as it will be always ready, but we will need start bit
 * from the HMI_ECU as it is slower and may be not ready yet when the
 * CONTROL_ECU starts to send using UART
 */
//#define CONTROL_ECU_READY	'C'
#define HMI_ECU_READY		'H'

#define TWI_ADDRESS		0x01
#define TWI_BITRATE		0x02

/*******************************************************************************
 *                                Global Variables                             *
 *******************************************************************************/

uint8 sec1 = 0; /* store seconds passed when opening the door */
uint8 sec2 = 0; /* store seconds passed when the error happened */

/*******************************************************************************
 *                                Timers CallBack Functions                    *
 *******************************************************************************/

/* Description:
 * rotate the motor to open close the door or stop it
 */
void CONTROL_TIMER1_count1(void)
{
	/* increment sec1 when timer is fired and ISR is called  */
	sec1++;

	/* rotate the motor or stop it according to the time passed */
	switch (sec1)
	{
	case 15:
		DcMotor_Rotate(STOP,0);
		break;
	case 18:
		DcMotor_Rotate(ACW,100);
		break;
	case 33:
		DcMotor_Rotate(STOP,0);
		Timer1_deInit();
		break;
	}
}
/* Description:
 * count seconds for the error function
 */
void CONTROL_TIMER1_count2(void)
{
	/* increment sec2 when timer is fired and ISR is called  */
	sec2++;
}

/*******************************************************************************
 *                                Functions definitions                        *
 *******************************************************************************/
/* Description:
 * function to send a byte to the HMI ECU
 */
void CONTROL_sendState (uint8 a_status)
{
	/* Wait until HMI_ECU is ready to receive the string */
	while(UART_receiveByte() != HMI_ECU_READY){}

	/* Send the required string to HMI_ECU through UART */
	UART_sendByte(a_status);
}

/* Description:
 * function to receive a byte from the HMI ECU
 */
uint8 CONTROL_receiveState(void)
{
	uint8 status;

	/* as the CONTROL_ECU is faster than the HMI_ECU. So, we won't need a start bit
	 * from the CONTOL_ECU as it will be always ready, but we will need start bit
	 * from the HMI_ECU as it is slower and may be not ready yet when the
	 * CONTROL_ECU starts to send using UART
	 */
	/* Send HMI_ECU_READY byte to HMI_ECU to ask it to send the string */
	//UART_sendByte(CONTROL_ECU_READY);

	/* Receive String from HMI_ECU through UART */
	status = UART_receiveByte();

	return status;
}

/* Description:
 * function to receive a password from the HMI ECU
 */
void CONTROL_receivePass(uint8 * a_pass)
{
	uint8 i;

	/* as the CONTROL_ECU is faster than the HMI_ECU. So, we won't need a start bit
	 * from the CONTOL_ECU as it will be always ready, but we will need start bit
	 * from the HMI_ECU as it is slower and may be not ready yet when the
	 * CONTROL_ECU starts to send using UART
	 */
	/* Send CONTROL_ECU_READY byte to HMI_ECU to ask it to send the string */
	//UART_sendByte(CONTROL_ECU_READY);

	for(i=0; i<5; i++)
	{
		/* Receive String from HMI_ECU through UART */
		a_pass[i] = UART_receiveByte();
	}
}


/* Description:
 * function to compare two passwords and return the state
 */
uint8 CONTROL_compPass(uint8 * a_pass , uint8 * a_test)
{
	uint8 i;
	uint8 status;

	for(i=0; i<5; i++)
	{
		/* if any character was unmatched then return the status with unmatched state*/
		if (a_pass[i] != a_test[i])
		{
			status = UNMATCHED;
			break;
		}
		/* if the 5 characters were matched return matched in the status */
		if ( i == 4 )
		{
			status = MATCHED;
		}
	}

	return status;
}

/* Description:
 * function to check an input pass send by HMI ECU with the stored pass in the EEPROM
 */
uint8 CONTROL_checkPass(void)
{
	uint8 status,i;
	uint8 comp[5]; /* store pass from the EEPROM */
	uint8 pass[5]; /* store pass from HMI ECU */


	for (i=0; i<5 ; i++)
	{
		/* read password from the external EEPROM */
		EEPROM_readByte(0x0311+i, &comp[i]);
		_delay_ms(10);
	}

	for (i=0 ; i<3 ; i++)
	{
		/* receive pass from HMI ECU */
		CONTROL_receivePass(pass);
		/* compare the received pass with the pass stored in EEPROM */
		status = CONTROL_compPass(pass ,comp);

		/* check if the password is matched */
		if (status == MATCHED)
		{
			CONTROL_sendState(status);
			break;
		}
		/* check if the password wasn't matched for 3 times */
		else if ( i ==2 )
		{
			status = COMPARE_ERROR;
			CONTROL_sendState(status);
			break;
		}
		/* send the state if unmatched */
		else
		{
			CONTROL_sendState(status);
		}
	}

	return status;
}

/* Description:
 * function to handle Error by turning the buzzer on for 60 second
 */
void CONTROL_error(void)
{
	/* turn on the buzzer */
	Buzzer_on();

	/* set the timer callback function */
	Timer1_setCallBack(CONTROL_TIMER1_count2);
	/* Timer1 Configuration */
	Timer1_ConfigType timerType = { 0 , 31250 , PRESCALER_256, COMPARE_MODE};
	/* Timer1 initialization */
	Timer1_init(&timerType);

	/* wait for 60 second */
	while (sec2 != 60){}

	/* clear the sec1 variable */
	sec2 = 0;

	/* turn off the buzzer */
	Buzzer_off();

	/* De-initialize the timer */
	Timer1_deInit();
}

/* Description:
 * 1. function to get the two passwords from the HMI ECU and compare them and send the
 * status to the HMI ECU
 * 2. store the password in the EEPROM
 */
void CONTROL_storePass()
{
	uint8 i;
	uint8 pass[5];
	uint8 test[5];
	uint8 status = UNMATCHED;

	/* loop until the state of two passwords is matched */
	while (status == UNMATCHED)
	{
		/* receive the first password */
		CONTROL_receivePass(pass);
		/* receive the confirmation password */
		CONTROL_receivePass(test);

		/* compare the two passwords and get the status*/
		status = CONTROL_compPass(pass ,test);
		/* send the status to HMI ECU */
		CONTROL_sendState(status);
	}

	for (i = 0 ; i<5 ; i++)
	{
		/* Write password in the external EEPROM */
		EEPROM_writeByte(0x0311+i, pass[i]);
		_delay_ms(10);
	}
}

/* Description:
 * function to get the user choice either open door or change password
 */
uint8 CONTROL_mainOptions(void)
{
	uint8 option;
	uint8 flag = '0';

	/* receive the key pressed from the HMI ECU */
	option = CONTROL_receiveState();

	switch (option)
	{
	case '+':
		flag = '1';
		break;
	case '-':
		flag= '2';
		break;
	}

	/* send the option to the HMI ECU */
	CONTROL_sendState(flag);

	return flag;
}


/* Description:
 * check the two passwords from the HMI ECU to decide either open the
 * door or go to error if the password was incorrect for 3 times
 */
void CONTROL_openDoor(void)
{
	uint8 status;

	/* check the entered passwords state */
	status = CONTROL_checkPass();

	switch (status)
	{
	/* check if the password is matched to go to open the door */
	case MATCHED:
		/* rotate teh motor to open the door */
		DcMotor_Rotate(CW,100);

		/* store the call back function */
		Timer1_setCallBack(CONTROL_TIMER1_count1);
		/* Timer1 Configuration */
		Timer1_ConfigType timerType = { 0 , 31250 , PRESCALER_256, COMPARE_MODE};
		/* Timer1 initialization */
		Timer1_init(&timerType);

		/* wait until the door is opened and entered and then closed to get out from the loop */
		while (sec1 != 33){}

		/* clear the sec1 variable */
		sec1 = 0;

		break;

	/* if the password is unmatched for 3 times then the status variable will have value of
	 * COMPARE ERROR and then go to the error function
	 */
	case COMPARE_ERROR:
		CONTROL_error();
		break;
	}

}

/* Description:
 * check the two passwords from the HMI ECU to decide either change the
 * password or go to error if the password was incorrect for 3 times
 */
void CONTROL_changePass(void)
{
	uint8 status;

	/* check the entered passwords state */
	status = CONTROL_checkPass();

	switch (status)
	{
	/* check if the password is matched to go to change the password */
	case MATCHED:
		CONTROL_storePass();
		break;

	/* if the password is unmatched for 3 times then the status variable will have value of
	 * COMPARE ERROR and then go to the error function
	 */
	case COMPARE_ERROR:
		CONTROL_error();
		break;
	}

}


int main (void)
{
	/* enable interrupt for the timer function*/
	SREG |= (1<<7) ;

	/* UART Configuration */
	UART_ConfigType uartType = {EIGHT_BIT, DISABLED, ONE_BIT, 9600};
	/* UART initialization */
	UART_init(&uartType);

	/* TWI Configuration */
	TWI_ConfigType twiType = {TWI_BITRATE, TWI_ADDRESS};
	/* TWI initialization */
	TWI_init(&twiType);

	/* initializing DcMotor */
	DcMotor_init();

	/* initializing buzzer */
	Buzzer_init();

	/* compare passwords and store it in the EEPROM at the start */
	CONTROL_storePass();
	for (;;)
	{
		switch (CONTROL_mainOptions())
		{
		/* if the user choose '+' then go to open door function */
		case '1':
			CONTROL_openDoor();
			break;

		/* if the user choose '-' then go to change password function */
		case '2':
			CONTROL_changePass();
			break;

		/* else, ask the user to enter the option he want again by
		 * repeating the loop */
		}
	}

}
