

#include "enemy.h"
#include "usart.h"

#define NUM_ENEMY 3

unsigned char NumEnemyCleared=0;
unsigned char Dragon_Pos = 0;
	
//index 0 represent the Horse, index1 is the man, index2 represents the soldier
unsigned short BitVector_Enemy[3]={0}; //uses the first 10 bits to hold enemy position
unsigned short BitVector_Dragon = 0;


//***********************************************************************//
//                         Function to Set Enemy                        //
//***********************************************************************//
void Enemy_Init(unsigned short EnemyType) {
	
	//Initializes the enemy position start at column 1
	if (EnemyType <=3) {
		BitVector_Enemy[2]|=(0x0200); //add a soldier
	}
	else {
		BitVector_Enemy[0]|=(0x0200); //add a knight
		BitVector_Enemy[1]|=(0x0200);
	}
	
}

//***********************************************************************//
//                         Function to Make Enemy Move                  //
//***********************************************************************//
void Move_Enemy(unsigned char Reset) {
	
	static unsigned char Enemies_Entered = 0;
	
	//if reset button is pressed, reassign local static variables
	if(Reset) {Enemies_Entered=0;}
	
	for (unsigned char i=0; i<NUM_ENEMY; i++) {
		
		BitVector_Enemy[i] = (BitVector_Enemy[i] >> 0x0001);
		
		//increments the number of enemies which enters the castle
	    if( (i>0) && (BitVector_Enemy[i] & (0x0001)) ) {
			Enemies_Entered++;
			SendTxData(0xF1);
		}
	}
	
	if(Enemies_Entered>=10) { SendTxData(0xFF); Enemies_Entered=0; } //send the You Lose command through usart
}

//***********************************************************************//
//                         Function to Show Enemy                        //
//***********************************************************************//
unsigned char Show_Enemy(unsigned char index) {
	
	(index==2) ? (TurnOnRedRow(1)) : (TurnOnRedRow(index+1));
	TurnColorToOrange();
	
	//loops through vector to check which bits are high.
	//Don't clear those pixel at those bits
	
	//don't show the enemy at the door
	TurnOffColumn(1);
	
	for (unsigned char k=10; k>1; k--) {
		// Turn off the pixels which aren't being used
		if ( !(BitVector_Enemy[index] & (0x0001<<(k-1))) ){TurnOffColumn(k);}
	}
	
	return (++index); //index changes from 0 to NUM_ENEMY-1
}

//***********************************************************************//
//                         Function to Show Soldiers                     //
//***********************************************************************//
unsigned char Clear_Enemy() {
	
	unsigned char temp = 0;
	
	//Clears the enemy if it gets hit
	for (unsigned char i=0; i<3; i++) {
		if ((y_pos==1) && (BitVector_Enemy[i] & (0x0001<<(x_pos-1)))) {
			//clear the bit if ballistic hit ground and soldier
			BitVector_Enemy[i] &= (~(0x0001<<(x_pos-1)));
			
			//make sure to only update this once
			if (temp==0) {
				NumEnemyCleared++; // each time enemy is hit, increment clear, summons dragon if >10
				temp=1;
			}
		}
	}
	
	//Clears the Man riding the Horse
	if ((y_pos==2) && (BitVector_Enemy[1] & (0x0001<<(x_pos-1)))) {
		//clear the bit if ballistic hit ground and soldier
		BitVector_Enemy[1] &= (~(0x0001<<(x_pos-1)));
	}
	
	return NumEnemyCleared;	
}

//***********************************************************************//
//                         Function to initialize Dragon                 //
//***********************************************************************//
void Summon_Dragon() {
	BitVector_Dragon|=(0x0200);
}

//***********************************************************************//
//                 Function to Move Dragon  Up/Down                      //
//***********************************************************************//
unsigned char Move_Dragon(int position_random) {
	
	//set the position of dragon randomly
	Dragon_Pos = position_random%3;
	
	//show where dragon is
	TurnOnGreenRow(Dragon_Pos+1);
	TurnColorToOrange();
	for (unsigned char i=2; i<=9; i++) { TurnOffColumn(i);}
	
	return (Dragon_Pos+3);
}

//***********************************************************************//
//                Boolean to Check if User got Hit                       //
//***********************************************************************//
unsigned char Hit_By_Dragon() {
	
	//If attack is at edge of screen and same position as the cannon
	if (y_pos == (Dragon_Pos+1)) {
	   SendTxData(0xF0);
	   return 1;
	}
	return 0;
}

//***********************************************************************//
//                 Function to make Dragon attack                        //
//***********************************************************************//
unsigned char Dragon_Attack(unsigned char Reset) {
	static unsigned short dragonfire = 0x0100;
	static unsigned char index_count = 0;
	static unsigned char life_counter = 0;
	
		
	//if reset button is pressed, reassign local static variables
	if(Reset) {dragonfire=0; index_count=0; life_counter=0;}
			
	unsigned char signal_dead = 0xFF; //used for USART to tell slave, User Loses!
	
	BitVector_Dragon = (dragonfire >> index_count) | (0x0200);
	
	if(index_count>=9) {
		//if 3 lives are lost then send dead signal to slave
		if( (life_counter+=Hit_By_Dragon())>=3) {SendTxData(signal_dead); life_counter=0;} ///HOW TO CALL SEND TX DATA HERE??
	}
	
	//move the attack across the screen
	if(index_count<10) {
		index_count++;
	}
	else {
		//reset index to attack again
		index_count=0;	
    }
	return (index_count);
}

//***********************************************************************//
//                 Function to show dragon on LED MATRIX                 //
//***********************************************************************//
void Show_Dragon(unsigned attack_pos) {
	TurnOnGreenRow(attack_pos);
	TurnColorToOrange();
	for (unsigned char k=1; k<=10; k++) {
		if ( !(BitVector_Dragon & (0x0001<<(k-1))) ){TurnOffColumn(k);}
	}
}
