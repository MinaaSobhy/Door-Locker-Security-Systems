/*
 * uart.h
 *
 *  Created on: Oct 27, 2022
 *      Author: Mina sobhy
 *      description: header file for UART
 */

#ifndef UART_H_
#define UART_H_

#include "std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
  FIVE_BIT, SIX_BIT, SEVEN_BIT, EIGHT_BIT, NINE_BIT = 7
} UART_BitData ;

typedef enum
{
  DISABLED, EVEN_PARITY = 2, ODD_PARITY = 3
} UART_Parity ;

typedef enum
{
  ONE_BIT, TWO_BIT
} UART_StopBit;

typedef uint32 UART_BaudRate;

typedef struct
{
	UART_BitData	bit_data	;
	UART_Parity		parity		;
	UART_StopBit	stop_bit	;
	UART_BaudRate 	baud_rate	;

} UART_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType * Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 a_data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_receiveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *STR_PTR);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *STR_PTR); // Receive until #

#endif /* UART_H_ */
