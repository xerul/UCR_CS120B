
// Permission to copy is granted provided that this header remains intact.
// This software is provided with no warranties.

/*** Modify by Rex Lu to use TX1 and RX1 , pins 16 and 17 on the Atmega1284 **/
////////////////////////////////////////////////////////////////////////////////

#ifndef USART_H
#define USART_H

// USART Setup Values
#define F_CPU 8000000UL // Assume uC operates at 8MHz
#define BAUD_RATE 9600
#define BAUD_PRESCALE (((F_CPU / (BAUD_RATE * 16UL))) - 1)

////////////////////////////////////////////////////////////////////////////////
//Functionality - Initializes TX and RX on PORT D
//Parameter: None
//Returns: None
void initUSART(unsigned char choose)
{
	// Turn on the reception circuitry
	// Use 8-bit character sizes - URSEL bit set to select the UCRSC register
	// Turn on receiver and transmitter
	if (choose==0) {
		UCSR1B |= (1 << RXEN1); 
	}
	else if (choose==1) {
		UCSR1B |= (1 << TXEN1);
	}
	else {
		UCSR1B |= (1 << RXEN1)  | (1 << TXEN1);
	}
	
	//sets register to work with 8 bits in synchronous
	UCSR1C |= (1 << UMSEL10)	| (1 << UCSZ10) | (1 << UCSZ11); 
	// Load lower 8-bits of the baud rate value into the low byte of the UBRR register
	UBRR1L = BAUD_PRESCALE;
	// Load upper 8-bits of the baud rate value into the high byte of the UBRR register
	UBRR1H = (BAUD_PRESCALE >> 8);
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - checks if USART is ready to send
//Parameter: la 
//Returns: 1 if true else 0
unsigned char USART_IsSendReady()
{
	return (UCSR1A & (1 << UDRE1));
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - checks if USART has recieved data
//Parameter: None
//Returns: 1 if true else 0
unsigned char USART_HasTransmitted()
{
	return (UCSR1A & (1 << TXC1));
}
////////////////////////////////////////////////////////////////////////////////
// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! ****
//Functionality - checks if USART has recieved data
//Parameter: None
//Returns: 1 if true else 0
unsigned char USART_HasReceived()
{
	return (UCSR1A & (1 << RXC1));
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - Flushes the data register
//Parameter: None
//Returns: None
/*
void USART_Flush()
{
	static unsigned char dummy;
	while ( UCSR1A & (1 << RXC1) ) { dummy = UDR1; }
}*/
////////////////////////////////////////////////////////////////////////////////
// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! ****
//Functionality - Sends an 8-bit char value
//Parameter: Takes a single unsigned char value
//Returns: None
void USART_Send(unsigned char sendMe)
{
	while( !(UCSR1A & (1 << UDRE1)) );
	UDR1 = sendMe;
}
////////////////////////////////////////////////////////////////////////////////
// **** WARNING: THIS FUNCTION BLOCKS MULTI-TASKING; USE WITH CAUTION!!! ****
//Functionality - receives an 8-bit char value
//Parameter: None
//Returns: Unsigned char data from the receive buffer
unsigned char USART_Receive()
{
	while ( !(UCSR1A & (1 << RXC1)) ); // Wait for data to be received
	return UDR1; // Get and return received data from buffer
}

//***********************************************************************//
//                Function to check if TxReady and Send Data             //
//***********************************************************************//
void SendTxData(unsigned char txdata) {
	while(!USART_IsSendReady()) {} //wait until usart is ready
	USART_Send(txdata); //sends the data over USART
	//while(!USART_HasTransmitted()) {} //wait until all data has transmitted
}

#endif //USART_H
