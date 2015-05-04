

#ifndef _JOYSTICKADC_
#define _JOYSTICKADC_

//***********************************************************************//
//                Analog to Digital Setup Function                       //
//***********************************************************************//

void ADC_init() {
	//Changes the resolution from 10 bits to 8 bits
	//ADMUX |= (1 <<ADLAR);
	
	/*Single Conversion Mode */
	ADCSRA |= (1 << ADEN) | (1 << ADSC);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	
	/**Not used: **/
	// ADATE: setting this bit enables auto-triggering. Since we are
	// in Free Running Mode, a new conversion will trigger whenever
	// the previous conversion completes.
}

//***********************************************************************//
//                Analog to Digital Channel Setup Function               //
//***********************************************************************//
unsigned short SetADC_Ch(unsigned char channel) { //Refer to Page 255 on Atmega1284 Data Sheet
	switch (channel) {
		//Set ADMUX[4:0] to 0000 for ADC0 channel reading
		
		case 0:
		ADCSRA &= ~(1 << ADEN);
		ADMUX&=0xF0;
		ADCSRA |= (1 << ADEN) | (1 << ADSC);
		while(ADCSRA & (1<<ADSC)) {/*do nothing, wait until ADC is Ready*/}
		break;
		
		//Set ADMUX[4:0] to 0001 for ADC1 channel reading
		case 1:
		ADCSRA &= ~(1 << ADEN);
		ADMUX|=0x01;
		ADCSRA |= (1 << ADEN) | (1 << ADSC);
		while(ADCSRA & (1<<ADSC)) {/*do nothing*/}
		break;
		
		default:
		ADCSRA &= ~(1 << ADEN);
		ADMUX=0;
		break;
	}
	
	return ADC;
}

//***********************************************************************//
//                     JoyStick for X-dir Calibration                    //
//***********************************************************************//
unsigned char JoyStickX_ADC(unsigned short joystick){
	//map 1024 to 0,1,2,3,4...10 [11]
	unsigned char offsetvalue=0;
	unsigned short division = (joystick>offsetvalue) ? ((joystick-offsetvalue)/93) : (0);
	return (division+1);
}

//***********************************************************************//
//                     JoyStick for Y-dir Calibration                    //
//***********************************************************************//
unsigned char JoyStickY_ADC(unsigned short joystick){
	//map 1024 to 0,1,2,3,4...6
	unsigned char offsetvalue=3;
	unsigned short division = (joystick>offsetvalue) ? ((joystick-offsetvalue)/146) : (0);
	return (division+1);
}

#endif
