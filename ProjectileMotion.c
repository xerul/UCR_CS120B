
#include <avr/io.h>
#include "ProjectileMotion.h"

//Ballistic Projectile Function
unsigned char x_pos=1; //initial firing distance
signed char y_pos=1; //initial firing height
unsigned char a = 0; //constant used for parabolic equation
unsigned char k = 0; //constant for parabolic equation

//***********************************************************************//
//                    Function to Fire Projectile from Cannon            //
//***********************************************************************//
unsigned char Compute_Ballistic(unsigned char cannon_height, unsigned char cannon_power) {
	
	/* Precomputed Distance using MATLAB for Horizontal Projectile Motion and truncation
	 * Based on Equation:
	 *  D = (v*sqrt(2*y*g)/g) + c
	 * where, v = velocity
	 *        g = gravity 10 [m/s^2]
	 *        c = offset constant (1)
    */
	const unsigned char CannonDistanceTable[MAX_ROW-1][MAX_COLUMN] =
	{
		//Values here are the final distance when making contact with ground
		//Rows: Shooting initial height from [2:7]
		//Columns: Shooting velocity/power from [1:10]
		{1,	2,	2,	3,	4,	4,	5,	6,	6,	7},
		{1,	2,	3,	4,	4,	5,	6,	7,	7,	8},
		{1,	2,	3,	4,	5,	6,	7,	8,	9,	9},
		{2,	3,	4,	5,	6,	7,	8,	9,	10,	11},
		{2,	3,	4,	5,	6,	7,	8,	9,	10, 11},
		{2,	3,	4,	5,	6,	8,	9,	10,	11,	12},

	};
	return CannonDistanceTable[cannon_height-1][cannon_power-1];
	
}

//***********************************************************************//
//                    Function to Set Up Projectile Shooting            //
//***********************************************************************//

void Initialize_Projectile(unsigned char cannon_height, unsigned char cannon_power) {
	unsigned char distance_desired = 0; //values from look up table of projectile motion trajectory
	unsigned char height_initial = cannon_height;
	unsigned short tempA=0;
	
	x_pos=1; //initialize horizontal trajectory
	y_pos=7; //initialize height
	distance_desired = Compute_Ballistic(cannon_height,cannon_power); //obtain final distance from look up table
	tempA=(height_initial*100)/(distance_desired*distance_desired); //multiply 'a' by 100 so that float would not need to be used
	a=tempA;
	k=height_initial; //height offset

}

//***********************************************************************//
//       Function to move trajectory of ballistic after shooting         //
//***********************************************************************//
void Update_Trajectory() {
		unsigned long tempY=0;
		
		x_pos++; //move to next pixel in x direction
		if ((y_pos<=1) || (y_pos>7) || (x_pos>10)){ //when x reaches end of screen or y overflows
			LED_Register=0;
			return;
		}
		
		//Parabolic Trajectory (-ax^2+bx+c)
		tempY=(a*x_pos*x_pos)/100;
		y_pos = (-tempY)+k;
		if (y_pos<1) {y_pos=1;}
		
}

//***********************************************************************//
//                    Function to Fire Projectile from Cannon            //
//***********************************************************************//
void Fire_Projectile() {
	ChooseColorRow(2,(unsigned char) y_pos);
	for (unsigned char i=1; i<=10; i++) { if(i!=x_pos){TurnOffColumn(i);}}
}
