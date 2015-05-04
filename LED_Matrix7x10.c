#include <avr/io.h>
#include "LED_Matrix7x10.h"

#define BIT 0x01
#define GREEN_BIT 0x00000100 //can't shift 0x01 << (num > 11)
#define RED_BIT 0x00040000

/*Must declare PORTC C0-C3 as outputs in Main*/

/*
      5 4 3 2 1 X 9 8 7 6
	7 x x x x x x x x x x 
	6 x x x x x x x x x x 
	5 x x x x x x x x x x 
	4 x x x x x x x x x x 
	3 x x x x x x x x x x 
	2 x x x x x x x x x x 
	1 x x x x x x x x x x 
	
*/

unsigned long LED_Register=0;
//0b 0000              0000 0000 00    00   0000 0000    1111 1111
//   N/A  Cathodes     Red  Red Red   Green Green Green     Anodes     
	

//***********************************************************************//
//                         Data Out to Shift Registers                   //
//***********************************************************************//
void ToLEDRegister(unsigned long data) {
	signed char i;
	
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;
	
	//4 shift registers = 32 bits
	for (i = 31; i >= 0; --i) { //LED Matrix Array uses 0
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTC = 0x08;
		// set SER = next bit of data to be sent.
		PORTC |= ((data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x02;
	}
	
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTC |= 0x04;
}

//***********************************************************************//
//                  Function to turn on Green Row  (1 only)              //
//***********************************************************************//
void TurnOnGreenRow(unsigned char row) { //takes 1-7 as parameter
	row--;
	
	//clear all rows
	LED_Register=0;
	
	if(row<=6) {
		LED_Register|=BIT<<(row);
	}
	else { //shouldn't happen unless error
		LED_Register=0;
	}
	LED_Register|=0xFFFC0000;
}

//***********************************************************************//
//                  Function to turn on Red Row  (1 only)              //
//***********************************************************************//
void TurnOnRedRow(unsigned char row) { //takes 1-7 as parameter
	row--;
	
	//clear all rows
	LED_Register&=0x00000000;
	
	if(row<=6) {
		LED_Register|=BIT<<(row);
	}
	else { //shouldn't happen unless error
		LED_Register=0;
	}
	LED_Register|=0x0003FF00;
}

//***********************************************************************//
//                  Function to turn on Orange Row  (1 only)              //
//***********************************************************************//
void TurnOnOrangeRow(unsigned char row) { //takes 1-7 as parameter
	row--;
	
	//clear all rows
	LED_Register&=0x00000000;
	
	if(row<=6) {
		LED_Register|=BIT<<(row);
	}
	else { //shouldn't happen unless error
		LED_Register=0;
	}
}

//***********************************************************************//
//  Function to turn off a column  (can be called multiple times)        //
//***********************************************************************//
void TurnOffColumn(unsigned char column) { //takes 1-10 as parameter
	
	unsigned long temp = 0;
	unsigned long green_temp = GREEN_BIT<<5; //need to do this, can't shift >7
	unsigned long red_temp = RED_BIT<<5; //need to do this, can't shift >7
	
	switch(column) {
		case 1: 
			column = 4; //maps column input to correct LED Column
			temp|=(GREEN_BIT<<column);
			temp|=(RED_BIT<<column);
			break;
		case 2:
			column = 3; //maps column input to correct LED Column
			temp|=(GREEN_BIT<<column);
			temp|=(RED_BIT<<column);
			break;
		case 3:
			column = 2; //maps column input to correct LED Column
			temp|=(GREEN_BIT<<column);
			temp|=(RED_BIT<<column);
			break;
		case 4:
			column = 1; //maps column input to correct LED Column
			temp|=(GREEN_BIT<<column);
			temp|=(RED_BIT<<column);
			break;
		case 5:		
			column = 0;
			temp|=(GREEN_BIT<<column);
			temp|=(RED_BIT<<column);
			break;
		case 6:
			column = 4;
			temp|=green_temp<<column;
			temp|=red_temp<<column;
			break;
		case 7:
			column = 3; //column relative to GREEN/RED_BIT
			temp|=green_temp<<column;
			temp|=red_temp<<column;
			break;
		case 8:
			column = 2;
			temp|=green_temp<<column;
			temp|=red_temp<<column;
			break;
		case 9:
			column = 1;
			temp|=green_temp<<column;
			temp|=red_temp<<column;
			break;
		case 10:
			column = 0;
			temp|=green_temp<<column;
			temp|=red_temp<<column;
			break;
		default:
			LED_Register=0;
			return;
	}

	LED_Register|=temp;
}

//***********************************************************************//
//                  Function to Set the Color of the Row                //
//***********************************************************************//
void ChooseColorRow(unsigned char color,unsigned char row) {
	
	//1=Red, 2=Green, 3=Orange
	
	switch(color) {
		case 1: 
			TurnOnRedRow(row);
			break;
		
		case 2:
			TurnOnGreenRow(row);
			break;
			
		case 3:
			TurnOnOrangeRow(row);
			break;
			
		default:
			LED_Register=0;
			break;
	}
}

//***********************************************************************//
//                  Function to convert one color to orange              //
//***********************************************************************//
void TurnColorToOrange() {
	//set all to orange except position 4
	LED_Register &= 0x000000FF;
}

//***********************************************************************//
//                  Function to set Row and Column  (1 only)              //
//***********************************************************************//
//TODO: FIX for multiple column and rows!
/*
unsigned long TurnOnRowColumnColor(unsigned char row, unsigned char column, unsigned char color) {
	//color: 1=Red, 2=Green, 3=Orange
	
	switch(color) {
		case 1:
			TurnOnRedRow(row);
			break;
		case 2:
			TurnOnGreenRow(row);
			break;
		case 3:
			TurnOnOrangeRow(row);
			break;
		default:
			LED_Register=0;
			return LED_Register;
			break;
	}
	
	//turn off 9 columns each time function gets called...might not be very efficient (TODO: FIX)
	for (unsigned char i=1; i<=10; i++) {
		if (i!=column) {
			TurnOffColumn(i);
		}
	}
	
	return LED_Register;
} */
