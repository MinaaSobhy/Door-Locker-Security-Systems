/*
 * hmi_main.c
 *
 *  Created on: Oct 29, 2022
 *      Author: Mina Sobhy
 *      Description: responsible for the interaction with the user just take inputs through
 *      			 keypad and display messages on the LCD
 */

#include "keypad.h"
#include "timer1.h"
#include "uart.h"
#include "lcd.h"
#include <util/delay.h>
#include <avr/io.h>

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define MATCHED 		'1'
#define UNMATCHED		'0'
#define COMPARE_ERROR	'2'


/* as the CONTROL_ECU is faster than the HMI_ECU. So, we won't need a start bit
 * from the CONTOL_ECU as it will be always ready, but we will need start bit
 * from the HMI_ECU as it is slower and may be not ready yet when the
 * CONTROL_ECU starts to send using UART
 */
//#define CONTROL_ECU_READY	'C'
#define HMI_ECU_READY		'H'  /* start bit send by the HMI_ECU when ready */

/*******************************************************************************
 *                                Global Variables                             *
 *******************************************************************************/
uint8 sec1 = 0; /* store seconds passed when opening the door */
uint8 sec2 = 0; /* store seconds passed when the error happened */

/*******************************************************************************
 *                                Timers CallBack Functions                    *
 *******************************************************************************/
/* Description:
 * print on the LCD the door state
 */
void HMI_TIMER1_count1(void)
{
	/* increment sec1 when timer is fired and ISR is called  */
	sec1++;

	/* print on the screen the current state according to the time passed */
	switch (sec1)
	{
	case 15:
		LCD_clearScreen();
		LCD_displayString("Entering");
		break;
	case 18:
		LCD_clearScreen();
		LCD_displayString("Door is Locking");
		break;
	case 33:
		LCD_clearScreen();
		Timer1_deInit();
		break;
	}

}
/* Description:
 * count seconds for the error function
 */
void HMI_TIMER1_count2(void)
{
	/* increment sec2 when timer is fired and ISR is called  */
	sec2++;
}


/*******************************************************************************
 *                                Functions definitions                        *
 *******************************************************************************/
/* Description:
 * function to send a password to the control ECU
 */
void HMI_sendPass (void)
{
	uint8 i;
	uint8 input[6];

	/* get the 5 characters of the password from the user */
	for (i=0; i<5; i++)
	{
		input[i] = KEYPAD_getPressedKey();
		LCD_displayString("*");
		_delay_ms(500);
	}

	/* loop until the user press enter button */
	while ( input[5] != '#')
	{
		input[5] = KEYPAD_getPressedKey();
	}

	/* as the CONTROL_ECU is faster than the HMI_ECU. So, we won't need a start bit
	 * from the CONTOL_ECU as it will be always ready, but we will need start bit
	 * from the HMI_ECU as it is slower and may be not ready yet when the
	 * CONTROL_ECU starts to send using UART
	 */
	/* Wait until Control_ECU is ready to receive the string */
	//while(UART_receiveByte() != CONTROL_ECU_READY){}

	for (i=0; i<5; i++)
	{
		/* Send the required string to CONTROL_ECU through UART */
		UART_sendByte(input[i]);
	}

}

/* Description:
 * function to send a byte to the control ECU
 */
void HMI_sendState (void)
{
	uint8 input ;


	input = KEYPAD_getPressedKey();
	_delay_ms(500);

	/* as the CONTROL_ECU is faster than the HMI_ECU. So, we won't need a start bit
	 * from the CONTOL_ECU as it will be always ready, but we will need start bit
	 * from the HMI_ECU as it is slower and may be not ready yet when the
	 * CONTROL_ECU starts to send using UART
	 */
	/* Wait until Control_ECU is ready to receive the string */
	/* while(UART_receiveByte() != CONTROL_ECU_READY){} */

	/* Send the required string to CONTROL_ECU through UART */
	UART_sendByte(input);
}

/* Description:
 * function to receive a byte from the control ECU
 */
uint8 HMI_receiveState(void)
{
	uint8 status;

	/* Send HMI_ECU_READY byte to CONTROL_ECU to ask it to send the string */
	UART_sendByte(HMI_ECU_READY);

	/* Receive String from CONTROL_ECU through UART */
	status = UART_receiveByte();

	return status;
}

/* Description:
 * function to print Error on the LCD and stop every thing else for 60 second
 */
void HMI_error(void)
{
	/* print error on the screen */
	LCD_clearScreen();
	LCD_displayString("     ERROR     ");

	/* set the timer callback function */
	Timer1_setCallBack(HMI_TIMER1_count2);
	/* Timer1 Configuration */
	Timer1_ConfigType timerType = { 0 , 15625 , PRESCALER_64, COMPARE_MODE};
	/* Timer1 initialization */
	Timer1_init(&timerType);

	/* wait for 60 second */
	while (sec2 !=60){}

	/* clear the sec1 variable */
	sec2 = 0;

	/* De-initialize the timer */
	Timer1_deInit();
}

/* Description:
 * function to get the two passwords from the user and wait until the two passwords are matched
 */
void HMI_createPass(void)
{
	/* variable to store the two password state */
	uint8 status = UNMATCHED;

	/* loop until the state of two passwords is matched */
	while (status == UNMATCHED)
	{
		/* displaying the first title to the user */
		LCD_clearScreen();
		LCD_displayString("Plz enter pass:");
		LCD_moveCursor(1,0);

		/* receive the first password from the user and send it to the control ECU */
		HMI_sendPass();

		/* displaying the first title to the user */
		LCD_clearScreen();
		LCD_displayString("Plz re-enter the");
		LCD_moveCursor(1,0);
		LCD_displayString("same pass: ");

		/* receive the confirm password from the user and send it to the control ECU */
		HMI_sendPass();

		/* receive the state of the two password from the control ECU */
		status = HMI_receiveState();
	}
}

/* Description:
 * function to get the user choice either open door or change password
 */
uint8 HMI_mainOptions(void)
{
	/* variable to store the chosin option */
	uint8 option;

	/* print the options on the screen */
	LCD_clearScreen();
	LCD_displayString("+ : Open Door");
	LCD_moveCursor(1,0);
	LCD_displayString("- : Change Pass");

	/*send the chosen option to the control ECU */
	HMI_sendState();

	/* receive the control ECU choice */
	option = HMI_receiveState();

	return option;
}


/* Description:
 * Receive the password from the user and send it to the CONTROL_ECU and check its condition
 * to decide either open the door or go to error if the password was incorrect for 3 times
 */
void HMI_openDoor(void)
{
	/* define variable to store the password state if it is correct or not and initialize it
	 * with unmatched at the beginning
	 */
	uint8 correct= UNMATCHED;

	/* still in the loop until the password is matched or the password was incorrect for 3 times */
	while (correct == UNMATCHED)
	{
		LCD_clearScreen();
		LCD_displayString("Plz enter pass:");
		LCD_moveCursor(1,0);

		/* receive the password from the user and send it to the CONTROL_ECU to check its state */
		HMI_sendPass();
		/* store the new password state */
		correct = HMI_receiveState();
	}

	switch (correct)
	{
	/* check if the password is matched to start opening the door*/
	case MATCHED:
		LCD_clearScreen();
		LCD_displayString("Door isUnlocking");

		/* store the call back function */
		Timer1_setCallBack(HMI_TIMER1_count1);
		/* Timer1 Configuration */
		Timer1_ConfigType timerType = {0 , 15625 , PRESCALER_64, COMPARE_MODE};
		/* Timer1 initialization */
		Timer1_init(&timerType);

		/* wait until the door is opened and entered and then closed to get out from the loop */
		while (sec1 != 33){}

		/* clear the sec1 variable */
		sec1 = 0;

		break;

	/* if the password is unmatched for 3 times then the correct variable will have value of
	 * COMPARE ERROR and then go to the error function
	 */
	case COMPARE_ERROR:
		HMI_error();
		break;
	}
}

/* Description:
 * Receive the password from the user and send it to the CONTROL_ECU and check its condition
 * to decide either change the password or go to error if the password was incorrect for 3 times
 */
void HMI_changePass(void)
{
	/* define variable to store the password state if it is correct or not and initialize it
	 * with unmatched at the beginning
	 */
	uint8 correct = UNMATCHED;

	/* still in the loop until the password is matched or the password was incorrect for 3 times */
	while (correct == UNMATCHED)
	{
		LCD_clearScreen();
		LCD_displayString("Plz enter pass:");
		LCD_moveCursor(1,0);

		/* receive the password from the user and send it to the CONTROL_ECU to check its state */
		HMI_sendPass();
		/* store the new password state */
		correct = HMI_receiveState();
	}

	switch (correct)
	{
	/* check if the password is matched to go to change the password */
	case MATCHED:
		HMI_createPass();
		break;

	/* if the password is unmatched for 3 times then the correct variable will have value of
	 * COMPARE ERROR and then go to the error function
	 */
	case COMPARE_ERROR:
		HMI_error();
		break;
	}
}



int main (void)
{
	/* define variable to store the condition of the main options menu */
	uint8 mainCondition;

	/* enable interrupt for the timer function*/
	SREG |= (1<<7) ;

	/* UART Configuration */
	UART_ConfigType uartType = {EIGHT_BIT, DISABLED, ONE_BIT, 9600};
	/* UART initialization */
	UART_init(&uartType);

	/* initializing LCD */
	LCD_init();

	/* create a password at the start */
	HMI_createPass();

	for (;;)
	{
		/* store the main menu option chosen */
		mainCondition = HMI_mainOptions();

		switch(mainCondition)
		{
		/* if the user choose '+' then go to open door function */
		case '1':
			HMI_openDoor();
			break;

			/* if the user choose '-' then go to change password function */
		case '2':
			HMI_changePass();
			break;

			/* else, ask the user to enter the option he want again by
			 * repeating the loop */
		}
	}
}

