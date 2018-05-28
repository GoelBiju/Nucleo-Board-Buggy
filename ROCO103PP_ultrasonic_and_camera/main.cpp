/*
Code for Buggy Motor Control and Microswitches, Ultrasonic sensor 
and Single pixel camera for the Nucleo Board (ROCO103PP).

Plymouth University
M.Simpson 31st October 2016
Edited 03/02/2017
*/

#include "mbed.h"
#include "motor.h"
#include "tunes.h"

// Define motor constants.
#define TIME_PERIOD 2             //Constant compiler Values here 2 equates to 2ms or 500Hz base Frequency
#define DUTY 0.9                  //DUTY of 1.0=100%, 0.4=40% etc.,

// Define ultrasonic constants.
#define RED 0
#define GREEN 1
#define BLUE 2

// Start (Blue User) button:
DigitalIn myButton(USER_BUTTON);  //USER_BUTTON is the Blue Button on the NUCLEO Board

// LED output:
DigitalOut led(LED1);             //LED1 is the Green LED on the NUCLEO board
                                  //N.B. The RED LED is the POWER Indicator
                                  //and the Multicoloured LED indicates status of the ST-LINK Programming cycle

// USB:
Serial pc(USBTX, USBRX);           //Instance of the Serial class to enable much faster BAUD rates then standard 9600 
																	// i.e. 115200
                                  // This is Pseudo RS232 over USB the NUCLEO will appear as a COMx Port see device Manager 
																	// on PC used. Use PuTTY to monitor check COMx and BAUD rate (115200).


// Microswitches:
DigitalIn microswitch1(D4);       //Instance of the DigitalIn class called 'microswitch1'
DigitalIn microswitch2(D3);       //Instance of the DigitalIn class called 'microswitch2'


// Motors:
Motor motor_A(D7,D8);             //Instances of the Motor Class see motor.h anf motor.cpp
Motor motor_B(D9,D10);            //They must be connected to these ports D7,D8 & D9,D10


// Ultrasonic:
DigitalOut Trigger(D6);						// Instance of the DigitalInOut class called 'TriggerEcho'.
DigitalIn Echo(D2);			

Timer pulse;											// Instance of the Timer class called 'pulse' so we can measure timed events.


// Single Pixel Camera:
//Instance of the AnalogIn class called 'ldr'
AnalogIn ldr(A1);                 

//Three coloured LEDs for the single Pixel Camera
DigitalOut red(D13);			  
DigitalOut green(D12);
DigitalOut blue(D11);


// Prototype functions for motor:
int motor(float speedA, float speedB);    //call as motor('Speed of MotorA Left','Speed of MotorB Right')
                                          //Where speed ranges from -1.0 to +1.0 inclusive rto give full reverse to full forward
                                          //And of course 0.0 will stop the Motor.

// Basic movement functions.
int faceLeft();
int faceRight();

//int reverseLeft();
//int reverseRight();

//int rotateClockwise();
//int rotateAntiClockwise();


void moveToObject();
void detectPaperColour();

// Prototype functions for ultrasonic sensor:
void ultra_sonic_distance(void);

float GetDistance(void);									


// Variables for motor.
// Variable 'duty' for programmer to use to vary speed as required set here to #define compiler constant see above
float duty=DUTY;

// Customised left and right duty to allow the buggy.
float normalLeftPower = -0.8;
float normalRightPower = 0.8;

float maxLeftPower = -0.9;
float maxRightPower = 0.91;

float maxLeftReversePower = 0.89;
float maxRightReversePower = -0.9;


// Variables for ultrasonic sensor.
float Distance;
float distance=0.0f;

// Variables for single pixel camera.
float red_light = 0.0f;
float green_light = 0.0f;
float blue_light = 0.0f;


// Entry point for program.
int main ()
{
	//BAUD Rate to 115200
	pc.baud(115200);            
  pc.printf("ROCO103PP Demonstration Robot Buggy Plymouth University 2018\n\r");

	// Motor Code:
	//Set frequency of the PWMs
  motor_A.Period_in_ms(TIME_PERIOD);    
  motor_B.Period_in_ms(TIME_PERIOD);
	
	// Ensure Motors are stopped - For the curious, this function is defined at the end of this file.
  motor(0.0f,0.0f);                     
  wait(0.5f);
	
	// Wait here for USER Button (Blue) on Nucleo Board (goes to zero when pressed).
	while (myButton == 1)
	{                                               
		wait(0.1);
  }
	
	
	bool objectDetected = false;
	
	// Ultrasonic Code:
	// Start the instance of the class timer.
	pulse.start();
	
	// Loop until an object is detected in a given range.
	while (objectDetected == false)
	{
		// Turn the motor slightly.
		motor(0, 1.0);
		wait(0.25f);
	
		motor(0, 0);
		wait(0.5f);
			
		// Get some accurate readings at each point.
		for (int i = 0; i < 4; i++)
		{
			distance = GetDistance();
			pc.printf("Object Distance: %dmm\n\r", (int)distance);
		}
		
		// Check if the object is in the given range.
		// If so then stop.
		if (distance < 600)
		{
			pc.printf("\nObject found, stopped, now moving to object.");
			objectDetected = true;
		}
		
		// If an object is detected..
		if (objectDetected)
		{
			// .. move to the object first.
			moveToObject();
			
			// Once we are at the object location, 
			// then detect the colour of the paper.
			detectPaperColour();
		}
	}
}


// Allow to move to the object whilst scanning that the object is in line.
void moveToObject()
{
	bool arrived = false;
	
	while (arrived == false)
	{
		// Get a set of new readings to see if the object
		// is still in line with the buggy.
		for (int i = 0; i < 4; i++)
		{
			distance = GetDistance();
			pc.printf("Arrival Distance: %dmm\n\r", (int)distance);
		}
		
		// Check if the object is in range and move towards it
		// as long as it still not close enough.
		if (distance < 500 && distance >= 20)
		{
			motor(-0.8, 0.77);
			wait(1.0f);
			
			motor(0, 0);
			wait(0.1f);
		}
		else if (distance < 30)
		{
			// State that the buggy has arruved at the location.
			arrived = true;
			
			// Play a tune once it has arrived.
			close_encounter(1);
		}
		else
		{
			// Adjust motor to get the object in range again.
			motor(-0.75, 0.75);
			wait(0.5f);
			
			motor(0, 0);
		}
	}
}


// Run this procedure when the buggy is roughly 2cm away from the object.
void detectPaperColour()
{
	// Turn all the LEDs off to start.
	red = 0;
	green = 0;
	blue = 0;
	
	// Illuminate with red LED only.
	//red = 1;
	//green = 0;
	//blue = 0;
	//red_light = ldr * 3.3f / 1.61;
	//pc.printf("\nRED LDR %4.2fV\n\r", red_light);
	//wait(0.5f);
	
	// Illuminate with green LED only.
	//red = 0;
	//green = 1;
	//blue = 0;
	//green_light = ldr * 3.3f / 1.33;
	//pc.printf("\nGREEN LDR %4.2fV\n\r", green_light);
	//wait(0.5f);
	
	
	// Illuminate with blue LED only.
	red = 0;
	green = 0;
	blue = 1;
	blue_light = ldr * 3.3f / 1.50;
	pc.printf("\nBLUE LDR %4.2fV\n\r", blue_light);
	wait(0.5f);
	
	
	// Decide if the paper being held in front is red or blue:
	// 1.40 is a threshold value which is the barrier between 
	// both the red and blue paper.
	if (blue_light < 1.45)
	{
		pc.printf("\nThe paper is blue.");
		blue = 0;
		
		wait(1.0f);
		blue = 1;
		wait(5.0f);
		blue = 0;
	}
	else if (blue_light > 1.45)
	{
		pc.printf("\nThe paper is red.");
		blue = 0;
		
		wait(1.0f);
		red = 1;
		wait(5.0f);
		red = 0;
	}
}
	


// Procedure to make the buggy face to the left of its 
// current position.
int faceLeft()
{
	// Give power to the right motor to turn whilst the left is powerless.
	motor(-0.825, 0);
	// Wait for it to finish turning.
	wait(2.0f);
	
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
	wait(1.5f);
	
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
	//CHECK speedA Value is in Range!
	if(speedA>1.0f||speedA<-1.0f)
	{ 
		//return ERROR code -1=speedA Value out of range! EXIT Function
		return -1;                  
  }
   
	//CHECK speedB Value is in Range!
	if(speedB > 1.0f || speedA < -1.0f)
	{ 
		//return ERROR code -2=speedB Value out of range! EXIT Function
		return -2;                  
	}

	//If speed values have passed the checks above then the following code will be executed
	if(speedA < 0.0f)
	{
	 motor_A.Rev(-speedA);
	}
	else
	{
	 motor_A.Fwd(speedA);
	}
	
	if(speedB < 0.0f)
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


// Loads the global variable distance with ultrasonic sensor value in mm 
// and then send the value to the stdio output i.e. serial over USB.
void ultra_sonic_distance(void)
{
	distance = GetDistance();
	
	// Adjust to get the correct reading from the sensor.
	// This is +5 millimeters in this case.
	distance += 5;
	
	pc.printf("%dmm \n\r", (int)distance);
}


// Returns the float value of the distance from the ultrasonic in millimeters.
float GetDistance()
{
	// Assign and set to zero the local variables for this function.
	float distance = 0.0;
	int EchoPulseWidth = 0;
	int EchoStart = 0;
	int EchoEnd = 0;
	
	// Initialise the values used and state for detection.
	// Signal goes High i.e. 3V3.
	Trigger = 1;
	
	// Wait 100us to give a pulse width triggering the ultrasonic module.
	wait_us(100);
	
	// Signal goes Low i.e. 0V.
	Trigger = 0;
	
	// Reset the instance of the Timer class.
	pulse.reset();
	
	
	// Wait for the Echo to go high.
	// Conditional 'AND' with timeout to prevent blocking.
	while (Echo == 0 && EchoStart < 25000)
	{
		EchoStart = pulse.read_us();
	}
	
	// Wait for the echo to return low.
	// Conditional 'OR' with timeout to prevent blocking.
	while (Echo == 1 && ((EchoEnd - EchoStart) < 25000))
	{
		EchoEnd = pulse.read_us();
	}
	
	
	// Calculate the time period; time period in us.
	EchoPulseWidth = EchoEnd - EchoStart;
	
	// Calculate the distance in mm and return the value as a float.
	distance = (float)EchoPulseWidth / 5.8f;
	
	//distance += 5.0;
	
	return distance;
}
