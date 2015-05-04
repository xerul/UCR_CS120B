
/* This is the Header File for the LED_Matrix7x10.c */

#ifndef _LED_Matrix7x10_H
#define _LED_Matrix7x10_H

void ToLEDRegister(unsigned long data);
void TurnOnGreenRow(unsigned char row);
void TurnOnRedRow(unsigned char row);
void TurnOnOrangeRow(unsigned char row); 
void TurnOffColumn(unsigned char column);
void ChooseColorRow(unsigned char color, unsigned char row);
void TurnColorToOrange();
//unsigned long TurnOnRowColumnColor(unsigned char row, unsigned char column, unsigned char color);

#endif
