/*
Author: Ze Cheng
Date: 22/11/2014
Description: ATmega2560 firmware for a rc_car and its servo platform
*/

/********************************************
*Include Libraries
*********************************************/
#include <Servo.h>
#include <StandardCplusplus.h>
#include <serstream>
#include <string>
#include <vector>
using namespace std;

#define INIT_X 95
#define INIT_Y 30



/********************************************
*Pin Assignment
*********************************************/
//DC Motor Control pins (H-Bridge input pins)
const int pin_Motor1 = 2; 
const int pin_Motor2 = 3;
const int pin_Motor3 = 4;
const int pin_Motor4 = 5;
const int motor_delay = 150;

//Servo Control Pins
const int pin_servoX = 6;
const int pin_servoY = 7; 

/********************************************
*Hardware Setting
*********************************************/

/*************Servo Settings****************/
Servo servoX;
Servo servoY;

//initial position
int localX = INIT_X;
int localY = INIT_Y;

//rotation speed
int speedX = 1;
int speedY = 1;

//max range for roration
const int maxX = 165;
const int maxY = 70;
const int minX = 25;
const int minY = 10;

/********************************************
*Utility
*********************************************/
int CheckChar(char inChar) {
  return ((inChar - '!') >= 0);
}



/********************************************
*Functions
*********************************************/
void motor_left () {
	digitalWrite(pin_Motor1,LOW);
	digitalWrite(pin_Motor2,LOW);
	digitalWrite(pin_Motor3,HIGH);
	digitalWrite(pin_Motor4,LOW);
}

void motor_right () {
	digitalWrite(pin_Motor1,LOW);
	digitalWrite(pin_Motor2,HIGH);
	digitalWrite(pin_Motor3,LOW);
	digitalWrite(pin_Motor4,LOW);
}

void motor_forward () {
	digitalWrite(pin_Motor1,LOW);
	digitalWrite(pin_Motor2,HIGH);
	digitalWrite(pin_Motor3,HIGH);
	digitalWrite(pin_Motor4,LOW);	
}

void motor_backward () {
	digitalWrite(pin_Motor1,HIGH);
	digitalWrite(pin_Motor2,LOW);
	digitalWrite(pin_Motor3,LOW);
	digitalWrite(pin_Motor4,HIGH);
}

void motor_off () {
	digitalWrite(pin_Motor1,LOW);
	digitalWrite(pin_Motor2,LOW);
	digitalWrite(pin_Motor3,LOW);
	digitalWrite(pin_Motor4,LOW);
}



/********************************************
*Initial Setup
*********************************************/
void setup() {
  //serial baud rate
  Serial.begin(9600);
  
 //initialize servo
  servoX.attach(pin_servoX);
  servoY.attach(pin_servoY);
  servoX.write(localX);
  servoY.write(localY);  
  
  //initialize motor
  pinMode(pin_Motor1, OUTPUT);
  pinMode(pin_Motor2, OUTPUT);
  pinMode(pin_Motor3, OUTPUT);
  pinMode(pin_Motor4, OUTPUT);
}


/********************************************
*Processing Control Signal
*********************************************/

char inChar;
char type;
String inMsg ="";

void loop() {
	if (Serial.available()) {
		inChar = Serial.read();
		if (inChar != ';') {
			if (CheckChar(inChar)) {inMsg.concat(inChar);}
		} else if (inChar == ';' && inMsg != "") {
			Serial.println("message received: " + inMsg);
			
			if (inMsg[0] == 'S') { //Servo control
				Serial.println("Servo Operation");
				int delimeter_index = inMsg.indexOf(',');
				if (delimeter_index != -1) {
					int inX = (inMsg.substring(1,delimeter_index)).toInt();
					int inY = (inMsg.substring(delimeter_index+1)).toInt();
							
					if (inX == 0) {
						servoX.write(localX);						
					} else {
						localX = inX;
						if (localX < maxX && localX > minX) { servoX.write(inX); delay(15);}
					}						
					if (inY == 0) {
						servoY.write(localY);						
					} else {
						localY = inY;
						if (localY < maxY && localY > minY) { servoY.write(inY); delay(15);}
					}							
				} else {
					if (inMsg[1] == 'g') {//servo sweep
						servoX.write(INIT_X);
						servoY.write(INIT_Y);
						while (localX > minX) {
							localX = localX - speedX;
							servoX.write(localX);
							delay(15);
						}
						while (localX < maxX) {
							localX = localX + speedX;
							servoX.write(localX);
							delay(15);
						}	
						while (localX > INIT_X) {
							localX = localX - speedX;
							servoX.write(localX);
							delay(15);
						}						
					}
					else if ((inMsg[1] == 'a') && (localX > minX)){//servo left
						localX = localX - speedX;
						servoX.write(localX);
					}       
					else if ((inMsg[1] == 'd') && (localX < maxX)){//servo right
						localX = localX + speedX;
						servoX.write(localX);
					}     
					else if ((inMsg[1] == 'w') && (localY < maxY)){//servo up
						localY = localY + speedY;
						servoY.write(localY);
					}       
					else if ((inMsg[1] == 's') && (localY > minY)){//servo down
						localY = localY - speedY;
						servoY.write(localY);
					}
				}			
			} else if (inMsg[0] == 'M') { //Motor control
				Serial.println("Motor Operation");
				if (inMsg[1] == 'w'){ 
					Serial.println("motor forward");
					motor_forward();
				} else if (inMsg[1] == 's') {
					Serial.println("motor backward");
					motor_backward();
				} else if (inMsg[1] == 'a') {
					Serial.println("motor turn left");
					motor_left();
				} else if (inMsg[1] == 'd') {
					Serial.println("motor turn right");
					motor_right();
				} else {
					Serial.print("motor stop ");
					motor_off();
				}			
			} else if (inMsg[0] == 'R') { //Reset
				servoX.write(INIT_X);
				servoY.write(INIT_Y);
			} else {
				Serial.println("invalid command");
			}
			inMsg = "";
		}
	} else {
		motor_off();
	}
}



