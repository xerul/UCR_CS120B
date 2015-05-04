/* Partner 1 Name & E-mail: Rex Lu, rlu003@ucr.edu
* Partner 2 Name & E-mail: Jose Rodriguez, jrodr050@ucr.edu
* Lab Section: 22
* Assignment: Final Project Main
*
* I acknowledge all content contained herein, excluding template or example code, is my own original work.
*/

//Game:
/********************************************************************************************************
The final project for CS120B is a Castle Defense Game.
- The purpose of the game is to defend your castle from infiltration by the enemy which is composed of 
soldiers and knights. You can control a cannon which can move vertically up or down and also adjust the 
initial firing power. The cannon is programmed to shoot using projectile motion physics in the horizontal 
direction. 
- After killing 10 enemies, a boss will appear. The boss is a flying dragon which moves randomly up or down.
The flying dragon will shoot fire balls at your cannon. If your cannon gets hit 3x by the fireball you will LOSE!
If there are 10 infiltrations by the soldiers and knights, you will also LOSE!
- In order to WIN, you have to kill 30 soldiers while dodging fireballs from the boss and preventing infiltration. 
********************************************************************************************************/

//Hardware:
/********************************************************************************************************
The hardware is composed of two ATmega1284 micro-controllers, one 2-axis analog Joystick, two 5x7 Bi-color
 LED DOT Matrix, four shift registers, seven Bipolar Junction Transistors, one 10xLED Bar, one 16x2 LCD
 screen, and two push-buttons.
********************************************************************************************************/


#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
#include "LED_Matrix7x10.c"
#include "JoyStickADC.c"
#include "ProjectileMotion.c"
#include "usart.h"
#include "enemy.c"

#define MAX_COLUMN 10
#define MAX_ROW 7

//***********************************************************************//
//                           Shared Variables                           //
//***********************************************************************//
unsigned long LEDmatrix=0;
unsigned char RowColor=1; //initialize to Red default
unsigned char cannon_power=0;
unsigned char cannon_height=0;
unsigned long ProjectilePosition=0; //holds LED Matrix Shift Register value
unsigned long SeederCount=0;
unsigned long ResetFlag=0;

//***********************************************************************//
//                         Function to do SoftReset                      //
//***********************************************************************//
void ResetEverything(){
		ResetFlag=1;
		
		/** All Global reset!  **/
		
		//main global
		LEDmatrix=0;
		RowColor=1;
		cannon_power=0;
		cannon_height=0;
		ProjectilePosition=0;
		SeederCount=0;
		//From enemy.c
		NumEnemyCleared=0;
		Dragon_Pos=0;
		BitVector_Enemy[0]=0;
		BitVector_Enemy[1]=0;
		BitVector_Enemy[2]=0; 
		BitVector_Dragon = 0;
		//From ProjectileMotion.c
		x_pos=1; 
		y_pos=1; 
		a = 0; 
		k = 0; 
		//From LED_Matrix7x10.c
		LED_Register=0;
		
}

//***********************************************************************//
//                         Function to Control LED Bars                  //
//***********************************************************************//
void LED_Bar(unsigned joystick_x) {
	//LED Bars goes from 1 (left) to 10 (right)
	
	unsigned char FirstHalfLED=0;
	unsigned char SecondHalfLED=0;
	
	//Set PORT to Low to turn On	
	if (joystick_x <= 5) {
	//Turn off PORTD, Turn on PORTB to show first 5 bars
		FirstHalfLED = ((0xFF)<<(joystick_x));
		SecondHalfLED = 0xFF;
		}
	else {
		/*if pass 5 bits, uses D port also. The shifting is due to where
		the wires are plugged in. Since PORTD4,D3,D2 are not used here. Reserved 
		for USART*/
		if (joystick_x > 7) {
			SecondHalfLED = (0xFF)<<(joystick_x-3);
			FirstHalfLED = 0xE0;
		}
		else {
			SecondHalfLED = (0xFF)<<(joystick_x-6);
			FirstHalfLED = 0xE0;
		}
	}
	
	PORTB = FirstHalfLED;
	PORTD = SecondHalfLED;
}

//***********************************************************************//
//                         Find GCD Function                             //
//***********************************************************************//
unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long c; //same thing as long int
	
	while(1) {
		c = a%b;
		if (c==0) {return b;}
		a = b;
		b = c;
	}
	return 0; //just cause the program to close if error
}

//***********************************************************************//
//                        Task Scheduler Data Structure                  //
//***********************************************************************//
typedef struct _task { 
/*Tasks should have members that include: state, period,
a measurement of elapsed time, and a function pointer.*/

	signed char state; //Task's Current State
	unsigned long int period; //Task Period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct) (int); //Task tick function
} task;


//***********************************************************************//
//                       User Defined State Machine                      //
//***********************************************************************//
//Enumeration of States.
enum SM1_States {LED_Multiplex=1};
enum SM2_States {Castle_Cannon=1,Castle_Door=2};
enum SM3_States {ReadYAnalogInput=1,ReadXAnalogInput=2};
enum SM4_States {Projectile_Init=1,ButtonRelease=2,Update_Projectile=3,Shoot_Projectile=4,Fire_Wait=5};
enum SM5_States {Initialize_Enemy=1,Enemy_Move=2,Show_EnemyLED=3};
enum SM6_States {Initialize_Dragon=1,Dragon_Move=2,Dragon_Fire=3,Dragon_Show=4};

//***********************************************************************//
//                      State Machine: Output to LED Matrix              //
//***********************************************************************//
int SMTick1 (int state) {


	//Read in Reset button, ATmega1284 inputs are pulled HIGH!
	unsigned char ResetButton = ((~PINA) & 0x08);
	
	//if reset is press, reassigns all variables and reinit states
	if(ResetFlag) {
		state=1;
	}
	
	switch(state) {
		case LED_Multiplex:
		    
			//if the reset button is held down, send the reset signal to slave and reassign all variables
		    if(ResetButton) {ResetEverything(); SendTxData(0xFE);}
			else {ResetFlag=0;}
				
		    SeederCount++; //count up seed so new random will be generated
			ToLEDRegister(LED_Register);
			state = LED_Multiplex;
			break;
			
		default:
			break;
		
	}
	return state;
}

//***********************************************************************//
//              State Machine: Castle and Cannon Control                //
//***********************************************************************//
int SMTick2 (int state) {	

	//Sets Castle Wall/Door on first column
	unsigned char castle_wall=1;
	
	//if reset is press, reassigns all variables
	if(ResetFlag) {
		state=1;
	}
	
	switch (state) {
		
		case Castle_Cannon:
			state=Castle_Door;
			break;
			
		case Castle_Door:
			state=Castle_Cannon;
			break;
		
		default:
			state=Castle_Cannon;
			break;
	}
	
	switch (state) {
		case Castle_Cannon:
			//Move Castle Cannon
			ChooseColorRow(2,cannon_height);
			for (unsigned char k=1; k<=10; k++) { if(k>1){TurnOffColumn(k);}}
			
			//Move LEDBAR	
			LED_Bar(cannon_power);
			break;
		
		case Castle_Door:
			//Set in Castle Wall and Door
			ChooseColorRow(1,castle_wall);
			for (unsigned char k=1; k<=10; k++) { if(k!=1){TurnOffColumn(k);}}
			//(castle_wall<7) ? (castle_wall++) : (castle_wall=0);
			break;	
		
		default:
			break;
			
	}

	return state;
}


//***********************************************************************//
//                   State Machine: Joystick Read                        //
//***********************************************************************//
int SMTick3 (int state) {
	
	//if reset is press, reassigns all variables
	if(ResetFlag) {
		state=1;
	}
	
	switch(state) {
	
		case ReadYAnalogInput:
		
		//SetADC is a function which chooses the channel of ADC to use
			cannon_height = JoyStickY_ADC(SetADC_Ch(0));
			if (cannon_height<=1) {cannon_height=2;} //set minimum height of cannon
			state=ReadXAnalogInput;
			break;
		
		case ReadXAnalogInput:
			cannon_power = JoyStickX_ADC(SetADC_Ch(1)); 		
			state=ReadYAnalogInput;
			break;
			
		default:
			break;
			
	}
	
	return state;	
}

//***********************************************************************//
//                 State Machine: Output Projectile Ballistic            //
//***********************************************************************//
int SMTick4 (int state) {
	static unsigned char counter=0;
	unsigned char Fire_Button = (~PINA) & 0x04;
	static unsigned char tempDataToSend = 0;
	
	//if reset is press, reinitialize all variables
	if(ResetFlag) {
		counter=0;
		tempDataToSend=0;
		state=1;
	}
	
	switch(state) {
		//set up firing trajectory
		case Projectile_Init:
			if (Fire_Button) {
				Initialize_Projectile(cannon_height,cannon_power);
				state=Update_Projectile;
			}
			else {
				state=Projectile_Init;
			}
			break;
			
		//update the trajectory
		case Update_Projectile:
			counter=0;
			Update_Trajectory(cannon_height,cannon_power);		
			SendTxData(tempDataToSend); //Sends data through USART, # of enemies killed
			state = (LED_Register==0) ? (Projectile_Init) : (Shoot_Projectile);		
			break;
		
		//show the ballistic on the LED Matrix
		case Shoot_Projectile:
			tempDataToSend=Clear_Enemy();
			Fire_Projectile();
			state = (counter>25) ? (Update_Projectile) : (Shoot_Projectile);
			counter++; //use counter to slow down the pixel
			break;
			
		default:
			break;
		
	}
	
	return state;
}

//***********************************************************************//
//                 State Machine: Enemy Movement                         //
//***********************************************************************//
int SMTick5 (int state) {
	     
		 
	    srand(SeederCount); //feed the random number generator
		
		static unsigned char Enemy_Spacing = 0; //how far apart each enemy is
		static unsigned short counter = 0;
		static unsigned char EnemyIndex = 0;
		unsigned char end_num = 5;
		unsigned char beg_num = 2;
		unsigned int random_number = 0; //calls the rand() function
		static unsigned int random_between = 0; //random between end and beg
		
		//if reset is press, reassigns all variables
		if(ResetFlag) {
			Enemy_Spacing=0;
			counter=0;
			EnemyIndex=0;
			random_between=0;
			state=1;
		}

		switch (state)
		{
			case Initialize_Enemy:
			    random_number = rand();
		      	random_between=(random_number%(end_num-beg_num))+beg_num; 

				Enemy_Init(random_between);
				Enemy_Spacing=0;
				state = Enemy_Move;
				break;
				
			case Enemy_Move:
				Move_Enemy(ResetFlag);
				Enemy_Spacing++;
				counter=0;
				state = (Enemy_Spacing >= random_between) ? (Initialize_Enemy) : (Show_EnemyLED);
				break;
				
			case Show_EnemyLED:
				EnemyIndex=Show_Enemy(EnemyIndex)%3; //changes from 0,1,2
				state = (counter>150) ? (Enemy_Move) : (Show_EnemyLED);
				counter++;
				break;
								
			default:
				state=Initialize_Enemy;
				break;
		}
		
	return state;
}

//***********************************************************************//
//                 State Machine: Enemy Dragon                           //
//***********************************************************************//
int SMTick6 (int state) {
	
	//Summon dragon after clearing 10 enemies
	if (NumEnemyCleared<=10) {
		return state;
	}
	
	srand(SeederCount);
	static char attack_location = 0;
	static unsigned char count = 0;
	int random_dragon = 0;
	
	
 	//if reset is press, reassigns all variables
 	if(ResetFlag) {
	 	attack_location=0;
	 	count=0;
	    state=1;
 	}
	
	
	switch (state)
	{
		case Initialize_Dragon:
			 Summon_Dragon();
			 state=Dragon_Move;
			 break;
			
		case Dragon_Move:
		    random_dragon = rand();
			attack_location = Move_Dragon(random_dragon);
			state=Dragon_Fire;
			break;
			
		case Dragon_Fire:
			count = 0;
			state = (Dragon_Attack(ResetFlag)) ? (Dragon_Show) : (Dragon_Move);
			break;
		
		case Dragon_Show:
			Show_Dragon(attack_location);
			count++;
			state = (count>15) ? (Dragon_Fire) : (Dragon_Show);
			break;
	}
	
	return state;
}


//@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@#@
//***********************************************************************//
//                                MAIN                                  //
//***********************************************************************//
int main(void)
{
	DDRC = 0xFF; PORTC = 0x00; //output for ShiftRegister LEDMatrix
	DDRD = 0xFF; PORTD = 0x00; //Bits D0,D1 and D4-D7 are output for LED Bars 
	DDRB = 0x1F; PORTB = 0xE0; //Bits B0-B4 are output for LED Bars 
	DDRA = 0x00; PORTA = 0xFF; //Input Analog 
	
	//initializes the ADC input
	ADC_init();
	
	//Initializes the USART for transmitting data
	initUSART((unsigned char) 0x01);
	
	// Period for the tasks------------------------------------------------------------
	unsigned short int SMTick1_calc = 1; //1 ms to multiplex LED Matrix
	unsigned short int SMTick2_calc = 1; //1 ms to set multiplex each event in LED MAtrix
	unsigned short int SMTick3_calc = 25; //25 ms to record each Analog Input from Joystick
	unsigned short int SMTick4_calc = 3; //3ms to show projectile, counter will update position of projectile
	unsigned short int SMTick5_calc = 5; //5ms to show enemy, 500ms to move the enemy
	unsigned short int SMTick6_calc = 7; //7ms to show the dragon and shoot fireball randomly
	
	//Calculating GCD----------------------------------------------------------------
	unsigned long int tmpGCD = 1; //1 ms period
	
	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;
	
	//Recalculate GCD periods for scheduler---------------------------------------------
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	unsigned long int SMTick2_period = SMTick2_calc/GCD;
	unsigned long int SMTick3_period = SMTick3_calc/GCD;
	unsigned long int SMTick4_period = SMTick4_calc/GCD;
	unsigned long int SMTick5_period = SMTick5_calc/GCD;
	unsigned long int SMTick6_period = SMTick6_calc/GCD;
	
	//Declare an array of tasks-----------------------------------------------------------
	static task task1, task2, task3, task4, task5, task6;
	task *tasks[] = { &task1, &task2, &task3, &task4, &task5, &task6};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	// Task 1
	task1.state = 1; //Task initial state.
	task1.period = SMTick1_period; //Task Period.
	task1.elapsedTime = SMTick1_period; //Task current elapsed time.
	task1.TickFct = &SMTick1; //Function pointer for the tick.
	
	// Task 2
	task2.state = 1; //Task initial state.
	task2.period = SMTick2_period; //Task Period.
	task2.elapsedTime = SMTick2_period; //Task current elapsed time.
	task2.TickFct = &SMTick2; //Function pointer for the tick.
	
	// Task 3
	task3.state = 1; //Task initial state.
	task3.period = SMTick3_period; //Task Period.
	task3.elapsedTime = SMTick3_period; //Task current elapsed time.
	task3.TickFct = &SMTick3; //Function pointer for the tick.
	
	// Task 4
	task4.state = 1; //Task initial state.
	task4.period = SMTick4_period; //Task Period.
	task4.elapsedTime = SMTick4_period; //Task current elapsed time.
	task4.TickFct = &SMTick4; //Function pointer for the tick.
	
	// Task 5
	task5.state = 1; //Task initial state.
	task5.period = SMTick5_period; //Task Period.
	task5.elapsedTime = SMTick5_period; //Task current elapsed time.
	task5.TickFct = &SMTick5; //Function pointer for the tick.
	
	// Task 6
	task6.state = 1; //Task initial state.
	task6.period = SMTick6_period; //Task Period.
	task6.elapsedTime = SMTick6_period; //Task current elapsed time.
	task6.TickFct = &SMTick6; //Function pointer for the tick.
	
	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	
	unsigned short i; // Scheduler for loop iterator
	
	while(1) {
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if (tasks[i]->elapsedTime == tasks[i]->period) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	
	// Error: Program should not exit!
	return 0;
}


