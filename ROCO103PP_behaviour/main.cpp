/*
Simple Routine for Nucleo Board for ROCO103PP Buggy Motor COntrol and Microswitches
Plymouth University
M.Simpson 31st October 2016
Edited 03/02/2017
*/

#include "mbed.h"
#include "motor.h"
#include "tunes.h"

//Constant compiler Values here 2 equates to 2ms or 500Hz base Frequency.
//DUTY of 1.0=100%, 0.4=40% etc.,
#define TIME_PERIOD 2             
#define DUTY 0.9                  


// Initialise components:
DigitalIn microswitch1(D4);       //Instance of the DigitalIn class called 'microswitch1'
DigitalIn microswitch2(D3);       //Instance of the DigitalIn class called 'microswitch2'

Motor motor_A(D7,D8);             //Instances of the Motor Class see motor.h and motor.cpp
Motor motor_B(D9,D10);            //They must be connected to these ports D7,D8 & D9,D10

DigitalIn myButton(USER_BUTTON);  //USER_BUTTON is the Blue Button on the NUCLEO Board

DigitalOut led(LED1);             //LED1 is the Green LED on the NUCLEO board
                                  //N.B. The RED LED is the POWER Indicator
                                  //and the Multicoloured LED indicates status of the ST-LINK Programming cycle

Serial pc(USBTX,USBRX);           //Instance of the Serial class to enable much faster BAUD rates then standard 9600 i.e. 115200
                                  //This is Pseudo RS232 over USB the NUCLEO will appear as a COMx Port see device Manager on PC used
                                  //Use PuTTY to monitor check COMx and BAUD rate (115200)



int faceLeft();
int faceRight();

int reverseLeft();
int reverseRight();

int rotateClockwise();
int rotateAntiClockwise();

int pushSequence();


//The Following line is a Function Prototype
int motor(float speedA, float speedB);    //call as motor('Speed of MotorA Left','Speed of MotorB Right')
                                          //Where speed ranges from -1.0 to +1.0 inclusive rto give full reverse to full forward
                                          //And of course 0.0 will stop the Motor


//Variable 'duty' for programmer to use to vary speed as required set here to #define compiler constant see above
float duty=DUTY;

// Customised left and right duty to allow the buggy to perform better.
float normalLeftPower = -0.8;
float normalRightPower = 0.8;

float maxLeftPower = -0.9;
float maxRightPower = 0.91;
float maxLeftReversePower = 0.89;
float maxRightReversePower = -0.9;


// Customisable settings for the buggy to clean.
//float maximumCheckpoints = 1;  // The number of procedures to carry out.
//float checkpointInterval = 1.0f;  // The time it takes between from the end of the previous to the start of the next checkpoint.


int main ()
{
  pc.baud(115200);            //BAUD Rate to 115200
  pc.printf("ROCO103PP Demonstration Robot Buggy Plymouth University 2018\n\r");

  motor_A.Period_in_ms(TIME_PERIOD);    //Set frequency of the PWMs
  motor_B.Period_in_ms(TIME_PERIOD);
	
	
	
  //
  //--------------------------- Robot Buggy Strategy ---------------------------------------------	
  //
	
	// Ensure Motors are stopped - For the curious, this function is defined at the end of this file.
  motor(0.0f, 0.0f); 
	wait(0.5f);
	
	//tune to play Announce start; see tunes.h for alternatives.
  twinkle(1);


	//Wait here for USER Button (Blue) on Nucleo Board (goes to zero when pressed).
	while(myButton==1)
	{                 
		// Flash green LED whilst waiting.
		led=0;                                
		wait(0.1);
		led=1; 
		wait(0.1);
		
		
		// NOTE: Microswitches have not been attached in the buggy.
    if(microswitch1==1)
    {
        pc.printf("Switch1 = %4.2fV\n\r",(float)microswitch1*3.3f);//printing value of microswitch1 in PuTTy window on PC
                                                                  //NB this is a Digital Input and so returns a Boolean 1 or 0
                                                                  //and so 'cast' the result into a 'float' type and multiply by 3.3!
                                                                  // see the instruction doc on how to install putty.
        tone1();
    }
		
    //Test Microswitches with two different tones see tunes.cpp tunes.h
    if(microswitch2==1)
    {
        pc.printf("Switch 2 pressed\n\r");	//Another example of how to print a message telling about the program workings.
				
				tone2();
    }
  }
	
	
	
	// The buggy will execute a maximum of 3 times.
	for (int i = 0; i <= 2; i++)
	{
		// START:
		pushSequence();
		
		// Reverse from current position back to start.
		motor(maxLeftReversePower, maxRightReversePower);
		wait(4.0f);
		motor(0,0);
		wait(2.0f);
		
		// Turn on the spot to face left.
		rotateClockwise();
		wait(1.25f);
		
		// Move to next position and execute same as above.
		motor(maxLeftPower, maxRightPower);
		wait(2.0f);
		motor(0,0);
		wait(0.5f);
		
		// Turn on the spot to face right.
		rotateAntiClockwise();
		wait(1.0f);
	}
	
	
	// FINISH:
	close_encounter(1);
}
//
//----------------------------------------------------------------------------------------------
//


// The sequence to push beads up to the ridge.
// Initiate the pushing sequence:
// 1. accelerate to a maximum pushing speed
// 2. upon approaching the ridge, apply maximum power to servo
// 3. stop servo
// 4. reverse back and apply pressure and stop
// 5. stop servo and proceed to next checkpoint.
int pushSequence()
{
	// Push against beads.
	motor(maxLeftPower, maxRightPower);
	wait(1.75f);
	motor(0,0);
	wait(1.0f);
	
	motor(maxLeftPower, maxRightPower);
	wait(1.0f);
	motor(0,0);
	wait(1.0f);
	
	motor(maxLeftPower, maxRightPower);
	wait(0.75f);
	motor(0,0);
	wait(1.0f);
	
	motor(-1.0, 1.0);
	wait(1.75f);
	motor(0,0);
	wait(2.0f);
	
	return 0;
}


// Procedure to make the buggy face to the left of its 
// current position.
int faceLeft()
{
	// Give power to the right motor to turn whilst the left is powerless.
	motor(-0.825, 0);
	// Wait for it to finish turning.
	wait(1.0f);
	
	// Stop the motor and wait for it to power down.
	motor(0, 0);
	wait(1.0f);
	
	return 0;
}


// Procedure to make the buggy face to the right of its
// current position.
int faceRight()
{
	
	motor(0, 0.85);
	wait(2.5f);
	
	motor(0, 0);
	wait(1.0f);
	
	return 0;
}


// Procedure to make the buggy reverse and face left
// from the current position.
int reverseLeft()
{
	// maximum duty to the servo
	motor(0, -1.0);
	wait(2.25f);
	
	// stop and wait for the servo to finish
	motor(0, 0);
	wait(1.0f);
	
	return 0;
}


// Procedure to make the buggy reverse and face right 
// from the current position.
int reverseRight()
{
	// give maximum duty to the servo.
	motor(1.0, 0);
	wait(2.0f);
	
	// stop the servo and wait
	motor(0, 0);
	wait(1.0f);
	
	return 0;
}


// Rotates in a clockwise direction for approx. 90 degrees.
int rotateClockwise()
{
	motor(0.8, 0.8);
	wait(1.0f);
	
	motor(0, 0);
	wait(2.0f);
	
	return 0;
}


// Rotates in an anti-clockwise direction for approx. 90 degrees.
int rotateAntiClockwise()
{
	motor(-0.9, -0.9);
	wait(1.0f);
	
	motor(0, 0);
	wait(2.0f);

	return 0;
}


//Small function to control motors use as motor(1.0,-0.5) Motor A full speed forward Motor B half speed reversed
int motor(float speedA, float speedB)
{
	
		// CHECK speedA Value is in Range!
   if(speedA>1.0f||speedA<-1.0f)
	 {
			//return ERROR code -1=speedA Value out of range! EXIT Function		 
      return -1;                  
   }
	 
	 //CHECK speedB Value is in Range!
   if(speedB>1.0f||speedA<-1.0f)
	 {
			//return ERROR code -2=speedB Value out of range! EXIT Function
      return -2;                  
   }
   
	 //If speed values have passed the checks above then the following code will be executed:
   if(speedA<0.0f)
	 {
     motor_A.Rev(-speedA);
   }
   else
	 {
     motor_A.Fwd(speedA);
   }
	 
	 
   if(speedB<0.0f)
	 {
     motor_B.Rev(-speedB);
   }
   else
	 {
     motor_B.Fwd(speedB);
   }
	 
	 
	 //Return ERROR code Zero i.e. NO ERROR success!
   return 0;                      
}


/*      //Consider these lines of code to Accelerate the motors
//      for (float i=0.5f; i<=1.0f; i+=0.01f) //Accelerate  from 50% to 100%
//      { 
//        motor(i,i);
//        wait(0.1f);
//      }
*/
