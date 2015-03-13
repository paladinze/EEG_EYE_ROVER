/**********************************************************
*Ground Rover Communication Module
*SubModules
*	1.Eyetracker (UDP receiving end)
*	2.Serial Communication
*Author: Ze Cheng
*Liscence: Public Domain
***********************************************************/

/***********************************************************
Include
***********************************************************/
#include "BtSerial.h"
#include "ModeState.h"
#include "ServoState.h"
#include "MotorState.h"
#include "SensorComm.h"
#include "GameController.h"
#include <string>
#include <vector>
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <signal.h> //pre-termination cleanup
#include <Windows.h> //windows API header
#include <msclr\marshal_cppstd.h> //C++ support libraries

/***********************************************************
Namespace
***********************************************************/
#using <System.dll> //System dll for win32
using namespace std;
using namespace System;
using namespace System::Threading;
using namespace System::Collections::Generic;
using namespace System::Text;
using namespace System::Threading::Tasks;
using namespace System::Net;
using namespace System::Net::Sockets;

/***********************************************************
Global Varaibles
***********************************************************/

//const int UDPPORT = 11000;
const int UDPPORT = 1;
BtSerial* SerialHandle = new BtSerial("COM7");
ModeState* ModeHandle = new ModeState;
ServoState* ServoHandle = new ServoState;
MotorState* MotorHandle = new MotorState;
SensorComm* SensorHandle = new SensorComm;
GameController GamepadHandle;

/*******************************************
Exeeption Handling
*******************************************/
//Console Event handling
BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		Beep(750, 300);
		printf("Ctrl-C event\n\n");
		SerialHandle->close_port();
		exit(0);

		// CTRL-CLOSE: confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		Beep(600, 200);
		printf("Ctrl-Close event\n\n");
		SerialHandle->close_port();
		exit(0);

		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
		Beep(900, 200);
		printf("Ctrl-Break event\n\n");
		return FALSE;

	case CTRL_LOGOFF_EVENT:
		Beep(1000, 200);
		printf("Ctrl-Logoff event\n\n");
		return FALSE;

	case CTRL_SHUTDOWN_EVENT:
		Beep(750, 500);
		printf("Ctrl-Shutdown event\n\n");
		return FALSE;

	default:
		return FALSE;
	}
}



/***********************************************************
Modes Description & Communication protocl
***********************************************************/
/*
Connection Management:
	1.Home Key (connect/reconnect)
Mode Switching:
	1.Tab Key (change mode)

*MODE1: real-time keyboard  <= Default mode
	1.Camera Platform: WSAD
	2.Rover Platfrom: arrow key 
	3.One-time action:
		1.sweeping: G
		
*MODE2: real-time handsfree
	1.Two states:
		1.Moving forward
			1.reset camera position and remain static during operation
			2.turn if "quadrant left/right" detected from Eye tracker
		2.Observing
			1.vehicle remain static during operation
			2.camera follows eye movement
			3.Sweep
				1.Eye Blink
	2.State Swtiching:
		1.On/Off of a continuous muscle signal (clench teeth)
	3.Mode Switching:
		1.Engagement

*MODE3: Assited handsfree
	1.user observe surrounding
	2.confirm a detination (using long-term engagement)
	3.rover adjust and move to the postition
*/


/***********************************************************
*MODE1: real-time keyboard  <= Default mode
***********************************************************/
void Mode1MotorControl() {
	Console::WriteLine("\tEnable mode1 motor control");
	int speedForward = 4;
	int speedTurn = 4;
	int motorDelay = 0;
	float joystickThres = 0.5;
	while (ModeHandle->getMode() == 1) {
		if (GamepadHandle.isConnected) {
			if (GamepadHandle.ry_thumb > joystickThres) { //arrow_up
				Console::WriteLine("\t\tCar Forward");
				SerialHandle->write_port(MotorHandle->forward(), speedForward);
				Sleep(motorDelay);
			}
			else if (GamepadHandle.ry_thumb < -joystickThres) { //arrow_down
				Console::WriteLine("\t\tCar Backward");
				SerialHandle->write_port(MotorHandle->backward(), speedForward);
				Sleep(motorDelay);
			}
			else if (GamepadHandle.rx_thumb < -joystickThres) { //arrow_left
				Console::WriteLine("\t\tCar Left");
				SerialHandle->write_port(MotorHandle->left(), speedTurn);
				Sleep(motorDelay);
			}
			else if (GamepadHandle.rx_thumb > joystickThres) { //arrow_right
				Console::WriteLine("\t\tCar Right");
				SerialHandle->write_port(MotorHandle->right(), speedTurn);
				Sleep(motorDelay);
			}
		}
		else {
			if (GetAsyncKeyState(VK_UP) < 0) { //arrow_up
				Console::WriteLine("\t\tCar Forward");
				SerialHandle->write_port(MotorHandle->forward(), speedForward);
				Sleep(motorDelay);
			}
			else if (GetAsyncKeyState(VK_DOWN) < 0) { //arrow_down
				Console::WriteLine("\t\tCar Backward");
				SerialHandle->write_port(MotorHandle->backward(), speedForward);
				Sleep(motorDelay);
			}
			else if (GetAsyncKeyState(VK_LEFT) < 0) { //arrow_left
				Console::WriteLine("\t\tCar Left");
				SerialHandle->write_port(MotorHandle->left(), speedTurn);
				Sleep(motorDelay);
			}
			else if (GetAsyncKeyState(VK_RIGHT) < 0) { //arrow_right
				Console::WriteLine("\t\tCar Right");
				SerialHandle->write_port(MotorHandle->right(), speedTurn);
				Sleep(motorDelay);
			}
		}
		Sleep(100);
	}
}

void Mode1ServoControl() {
	Console::WriteLine("\tEnable mode1 servo control");
	int camDelay = 300;
	float joystickThres = 0;
	float maxSpeed = 3;
	float realSpeed = 0;
	while (ModeHandle->getMode() == 1) {
		
		if (GamepadHandle.isConnected) {
			//Servo: gamepad left joystick
			if (GamepadHandle.ly_thumb > joystickThres) { //arrow_up
				Console::WriteLine("CAM UP");
				realSpeed = maxSpeed * GamepadHandle.ly_thumb;
				SerialHandle->write_port(ServoHandle->tilt((int)round(realSpeed)));
			}
			else if (GamepadHandle.ly_thumb < -joystickThres) { //arrow_down
				Console::WriteLine("CAM DOWN");
				realSpeed = maxSpeed * GamepadHandle.ly_thumb;
				SerialHandle->write_port(ServoHandle->tilt((int)round(realSpeed)));
			}
			else if (GamepadHandle.lx_thumb < -joystickThres) { //arrow_left
				Console::WriteLine("CAM LEFT");
				realSpeed = maxSpeed * GamepadHandle.lx_thumb;
				SerialHandle->write_port(ServoHandle->pan((int)round(realSpeed)));
			}
			else if (GamepadHandle.lx_thumb > joystickThres) { //arrow_right
				Console::WriteLine("CAM RIGHT");
				realSpeed = maxSpeed * GamepadHandle.lx_thumb;
				SerialHandle->write_port(ServoHandle->pan((int)round(realSpeed)));
			}
		}
		else {
			//Servo: keyboard
			if (GetAsyncKeyState(0x52) < 0) {//Reset (R)
				SerialHandle->write_port(ServoHandle->reset());
				Sleep(camDelay);
				Console::WriteLine("\t\tCAM Reset");
			}
			else if (GetAsyncKeyState(0x47) < 0) {//sweep (G)
				SerialHandle->write_port(ServoHandle->sweep());
				Sleep(camDelay);
				Console::WriteLine("\t\tCAM Sweep");
			}
			else if (GetAsyncKeyState(0x57) <0) {//cam up
				if (ServoHandle->getY() > ServoHandle->defaultY) {
				}
				else if (ServoHandle->getY() == ServoHandle->defaultY) {
					SerialHandle->write_port(ServoHandle->setY(ServoHandle->defaultY + 25));
					Sleep(camDelay);
					Console::WriteLine("CAM UP");
				}
				else if (ServoHandle->getY() < ServoHandle->defaultY) {
					SerialHandle->write_port(ServoHandle->setY(ServoHandle->defaultY));
					Sleep(camDelay);
					Console::WriteLine("CAM UP");
				}
			}
			else if (GetAsyncKeyState(0x53) < 0) {//cam down
				if (ServoHandle->getY() < ServoHandle->defaultY) {
				}
				else if (ServoHandle->getY() == ServoHandle->defaultY) {
					SerialHandle->write_port(ServoHandle->setY(ServoHandle->defaultY - 15));
					Sleep(camDelay);
					Console::WriteLine("CAM DOWN");
				}
				else if (ServoHandle->getY() > ServoHandle->defaultY) {
					SerialHandle->write_port(ServoHandle->setY(ServoHandle->defaultY));
					Sleep(camDelay);
					Console::WriteLine("CAM DOWN");
				}
			}
			else if (GetAsyncKeyState(0x41) < 0) {//cam left
				if (ServoHandle->getX() < ServoHandle->defaultX) {
				}
				else if (ServoHandle->getX() == ServoHandle->defaultX) {
					SerialHandle->write_port(ServoHandle->setX(ServoHandle->defaultX - 35));
					Sleep(camDelay);
					Console::WriteLine("CAM LEFT");
				}
				else if (ServoHandle->getX() > ServoHandle->defaultX) {
					SerialHandle->write_port(ServoHandle->setX(ServoHandle->defaultX));
					Sleep(camDelay);
					Console::WriteLine("CAM LEFT");
				}
			}
			else if (GetAsyncKeyState(0x44) <0) {//cam right
				if (ServoHandle->getX() > ServoHandle->defaultX) {
				}
				else if (ServoHandle->getX() == ServoHandle->defaultX) {
					SerialHandle->write_port(ServoHandle->setX(ServoHandle->defaultX + 35));
					Sleep(camDelay);
					Console::WriteLine("CAM RIGHT");
				}
				else if (ServoHandle->getX() < ServoHandle->defaultX) {
					SerialHandle->write_port(ServoHandle->setX(ServoHandle->defaultX));
					Sleep(camDelay);
					Console::WriteLine("CAM RIGHT");
				}
			}

		}

		Sleep(100);
	}
}



/***********************************************************
*MODE2:real-time handsfree
***********************************************************/
//test eye control by predefined position


void Mode2RealTimeControl(){
	Console::WriteLine("\tEnable mode2 eye control");
	Console::WriteLine("\tEnable mode2 EEG control");
	Console::WriteLine("\tEntering defualt state: observation state");
	Console::WriteLine("\tHint: Use eyebrow to switch between observation and movement state");

	/*
	//move a fixed length on change
	int motorDelay = 10;
	int motorSpeed = 10;
	*/
	

	while (ModeHandle->getMode() == 2) {

		if (SensorHandle->getEEGEyebrowChange() == 1) {

			//Reset camera on state change
			//SerialHandle->write_port(ServoHandle->reset());
			//reset to look down
			SerialHandle->write_port(ServoHandle->setX(ServoHandle->defaultX));
			SerialHandle->write_port(ServoHandle->setY(ServoHandle->minY));


			Sleep(300);
			Console::WriteLine("\t\tCAM Reset");

			if (SensorHandle->getMoveObserveState() == 0){
				Console::WriteLine("\tEntering observation state");
			}
			else {
				Console::WriteLine("\tEntering movement state");
			}
			SensorHandle->setEEGEyebrowChange(0);
		}

		if (SensorHandle->getMoveObserveState() == 1) { //movement state
			if (SensorHandle->getEyeQuadrant() == "a") { //motor left
				int motorDelay = 100;
				int motorSpeed = 10;
				SerialHandle->write_port(MotorHandle->left(),motorSpeed);
				SensorHandle->setEyeQuadrant("n");
				Sleep(motorDelay);
			}
			else if (SensorHandle->getEyeQuadrant() == "d") {//motor right
				int motorDelay = 100;
				int motorSpeed = 10;
				SerialHandle->write_port(MotorHandle->right(),motorSpeed);
				SensorHandle->setEyeQuadrant("n");
				Sleep(motorDelay);
			}
			else if (SensorHandle->getEEGTeeth() == 1) {
				int motorDelay = 40;
				int motorSpeed = 1;
				SerialHandle->write_port(MotorHandle->forward(), motorSpeed);
				Sleep(motorDelay);
			}

			/*
			else if (SensorHandle->getEEGTeethChange() == 1) {//motor forward
				int motorDelay = 100;
				int motorSpeed = 15;
				SerialHandle->write_port(MotorHandle->forward(), motorSpeed);
				SensorHandle->setEEGTeethChange(0);
				Sleep(motorDelay);
			}

			*/

		}
		else if (SensorHandle->getMoveObserveState() == 0) { //Observation state
			int camDelay = 100;
			
			if (SensorHandle->getEEGPushChange() == 3) {// disable eeg push
				Console::WriteLine("Motor sweep");
				SerialHandle->write_port(ServoHandle->sweep());
				SensorHandle->setEEGPushChange(0);
				Sleep(4000);
			}
			else if (SensorHandle->getEyeActive() != 0) {
				if (SensorHandle->getEyeQuadrant() == "w") {//cam up
					if (ServoHandle->getY() > ServoHandle->defaultY) {
					}
					else if (ServoHandle->getY() == ServoHandle->defaultY) {
						SerialHandle->write_port(ServoHandle->setY(ServoHandle->defaultY + 25));
						Sleep(camDelay);
						Console::WriteLine("CAM UP");
					}
					else if (ServoHandle->getY() < ServoHandle->defaultY) {
						SerialHandle->write_port(ServoHandle->setY(ServoHandle->defaultY));
						Sleep(camDelay);
						Console::WriteLine("CAM UP");
					}
					SensorHandle->setEyeQuadrant("n");
				}
				else if (SensorHandle->getEyeQuadrant() == "s") {//cam down
					if (ServoHandle->getY() < ServoHandle->defaultY) {
					}
					else if (ServoHandle->getY() == ServoHandle->defaultY) {
						SerialHandle->write_port(ServoHandle->setY(ServoHandle->defaultY - 15));
						Sleep(camDelay);
						Console::WriteLine("CAM DOWN");
					}
					else if (ServoHandle->getY() > ServoHandle->defaultY) {
						SerialHandle->write_port(ServoHandle->setY(ServoHandle->defaultY));
						Sleep(camDelay);
						Console::WriteLine("CAM DOWN");
					}
					SensorHandle->setEyeQuadrant("n");
				}
				else if (SensorHandle->getEyeQuadrant() == "a") {//cam left
					if (ServoHandle->getX() < ServoHandle->defaultX) {
					}
					else if (ServoHandle->getX() == ServoHandle->defaultX) {
						SerialHandle->write_port(ServoHandle->setX(ServoHandle->defaultX - 35));
						Sleep(camDelay);
						Console::WriteLine("CAM LEFT");
					}
					else if (ServoHandle->getX() > ServoHandle->defaultX) {
						SerialHandle->write_port(ServoHandle->setX(ServoHandle->defaultX));
						Sleep(camDelay);
						Console::WriteLine("CAM LEFT");
					}
					SensorHandle->setEyeQuadrant("n");
				}
				else if (SensorHandle->getEyeQuadrant() == "d") {//cam right
					if (ServoHandle->getX() > ServoHandle->defaultX) {
					}
					else if (ServoHandle->getX() == ServoHandle->defaultX) {
						SerialHandle->write_port(ServoHandle->setX(ServoHandle->defaultX + 35));
						Sleep(camDelay);
						Console::WriteLine("CAM RIGHT");
					}
					else if (ServoHandle->getX() < ServoHandle->defaultX) {
						SerialHandle->write_port(ServoHandle->setX(ServoHandle->defaultX));
						Sleep(camDelay);
						Console::WriteLine("CAM RIGHT");
					}
					SensorHandle->setEyeQuadrant("n");
				}
				else if (SensorHandle->getEyeQuadrant() == "n") {
				};
				Sleep(10);
			}

		}

	}

}



void Mode2RealTimeControlVersion3() {
	Console::WriteLine("\tEnable mode2 eye control");
	Console::WriteLine("\tEnable mode2 EEG control");
	Console::WriteLine("\tDefualt state: observation state");

	int preTeeth = 0;
	int currTeeth = 0;

	std::string preDir = "n";
	std::string currDir = "n";

	while (1) {
		//Notify the state change
		if (preTeeth != currTeeth) {
			cout << "state Change" << endl;
			SensorHandle->setEEGTeethChange(1);
			SerialHandle->write_port(ServoHandle->reset()); //reset camera on change
		}
		preTeeth = currTeeth;

		//Entering the state
		if (SensorHandle->getEEGTeeth() == 1) { //Movement State
			currTeeth = 1;
			if (SensorHandle->getEyeQuadrant() == "a") {//A{
				SerialHandle->write_port(MotorHandle->left());			
			}
			else if (SensorHandle->getEyeQuadrant() == "d") {//D
				SerialHandle->write_port(MotorHandle->right());
			}
			else {
				//Console::WriteLine("\t\tCar Forward");
				//SerialHandle->write_port(MotorHandle->forward());
			}
			Sleep(50); // can't be slower than this
		}
		else if (SensorHandle->getEEGTeeth() == 0) { //Observation State
			currTeeth = 0;
			if (SensorHandle->getEEGEyebrow() == 1) {
				Console::WriteLine("Motor sweep");
				SerialHandle->write_port(ServoHandle->sweep());
				Sleep(4000);
			}
			else if (SensorHandle->getEyeActive() != 0) {
				if (SensorHandle->getEyeQuadrant() == "w") {//W
					if (ServoHandle->getY() > ServoHandle->defaultY) {
					}
					else if (ServoHandle->getY() == ServoHandle->defaultY) {
						SerialHandle->write_port(ServoHandle->setY(ServoHandle->defaultY + 25));
						Console::WriteLine("CAM UP");
					}
					else if (ServoHandle->getY() < ServoHandle->defaultY) {
						SerialHandle->write_port(ServoHandle->setY(ServoHandle->defaultY));
						Console::WriteLine("CAM UP");
					}
					SensorHandle->setEyeQuadrant("n");
				}
				else if (SensorHandle->getEyeQuadrant() == "s") {//S
					if (ServoHandle->getY() < ServoHandle->defaultY) {
					}
					else if (ServoHandle->getY() == ServoHandle->defaultY) {
						SerialHandle->write_port(ServoHandle->setY(ServoHandle->defaultY - 15));
						Console::WriteLine("CAM DOWN");
					}
					else if (ServoHandle->getY() > ServoHandle->defaultY) {
						SerialHandle->write_port(ServoHandle->setY(ServoHandle->defaultY));
						Console::WriteLine("CAM DOWN");
					}
					SensorHandle->setEyeQuadrant("n");
				}
				else if (SensorHandle->getEyeQuadrant() == "a") {//A
					if (ServoHandle->getX() < ServoHandle->defaultX) {
					}
					else if (ServoHandle->getX() == ServoHandle->defaultX) {
						SerialHandle->write_port(ServoHandle->setX(ServoHandle->defaultX - 35));
						Console::WriteLine("CAM LEFT");
					}
					else if (ServoHandle->getX() > ServoHandle->defaultX) {
						SerialHandle->write_port(ServoHandle->setX(ServoHandle->defaultX));
						Console::WriteLine("CAM LEFT");
					}
					SensorHandle->setEyeQuadrant("n");
				}
				else if (SensorHandle->getEyeQuadrant() == "d") {//D
					if (ServoHandle->getX() > ServoHandle->defaultX) {
					}
					else if (ServoHandle->getX() == ServoHandle->defaultX) {
						SerialHandle->write_port(ServoHandle->setX(ServoHandle->defaultX + 35));
						Console::WriteLine("CAM RIGHT");
					}
					else if (ServoHandle->getX() < ServoHandle->defaultX) {
						SerialHandle->write_port(ServoHandle->setX(ServoHandle->defaultX));
						Console::WriteLine("CAM RIGHT");
					}
					SensorHandle->setEyeQuadrant("n");
				}
				else if (SensorHandle->getEyeQuadrant() == "n") {
				};
				Sleep(10);
			}
		}
	}	
}


/****************************version 2***************************************************/
//test eye following 

void Mode2RealTimeControlVersion2() {
	Console::WriteLine("\tEnable mode2 eye control");
	Console::WriteLine("\tEnable mode2 EEG control");
	Console::WriteLine("\tDefualt state: observation state");

	int preTeeth = 0;
	int currTeeth = 0;
	int camSpeed = 10;

	while (1) {
		//Notify the state change
		if (preTeeth != currTeeth) {
			cout << "state Change" << endl;
			SensorHandle->setEEGTeethChange(1);
		}
		preTeeth = currTeeth;

		//Entering the state
		if (SensorHandle->getEEGTeeth() == 1) { //Movement State
			currTeeth = 1;
			if (SensorHandle->getEyeQuadrant() == "a") {//A{
				SerialHandle->write_port(MotorHandle->left());
				//Console::WriteLine("Eye LEFT => Car left");
			}
			else if (SensorHandle->getEyeQuadrant() == "d") {//D
				SerialHandle->write_port(MotorHandle->right());
				//Console::WriteLine("Eye RIGHT => Car right");
			}
			else {
				//Console::WriteLine("\t\tCar Forward");
				//SerialHandle->write_port(MotorHandle->forward());
			}
			Sleep(50); // can't be slower than this
		}
		else if (SensorHandle->getEEGTeeth() == 0) { //Observation State
			currTeeth = 0;
			if (SensorHandle->getEEGEyebrow() == 1) {
				Console::WriteLine("Motor sweep");
				SerialHandle->write_port(ServoHandle->sweep());
				Sleep(4000);
			}
			else if (SensorHandle->getEyeActive() != 0) {
				if (SensorHandle->getEyeQuadrant() == "w") {//W
					if (ServoHandle->getY() < ServoHandle->maxY) {
						SerialHandle->write_port(ServoHandle->tilt(+camSpeed));
					}
					SensorHandle->setEyeQuadrant("n");
				}
				else if (SensorHandle->getEyeQuadrant() == "s") {//S
					if (ServoHandle->getY() > ServoHandle->minY) {
						SerialHandle->write_port(ServoHandle->tilt(-camSpeed));
					}
					SensorHandle->setEyeQuadrant("n");
				}
				else if (SensorHandle->getEyeQuadrant() == "a") {//A
					if (ServoHandle->getX() > ServoHandle->minX) {
						SerialHandle->write_port(ServoHandle->pan(-camSpeed));
					}
					SensorHandle->setEyeQuadrant("n");
				}
				else if (SensorHandle->getEyeQuadrant() == "d") {//D
					if (ServoHandle->getX() < ServoHandle->maxX) {
						SerialHandle->write_port(ServoHandle->pan(+camSpeed));
					}
					SensorHandle->setEyeQuadrant("n");
				}
				else if (SensorHandle->getEyeQuadrant() == "n") {
				};
				Sleep(10);
			}
		}
	}
}

/***********************************************************
*MODE3:assisted handsfree
***********************************************************/
void Mode3AssistedControl() {
	Console::WriteLine("welcome to mode3! We are too lazy to implement it");

	/*procedure (basic version)
		0.Initialization
			1.camera reset to a fixed angle
		1.user:
			1.EYE observe screen
			2.EEG confirm eye gaze
			3.EYE send rotation angle
		2.Rover:
			1.receive angle
			2.rotate according to angle
		3.User:
			1.EEG to move forward;
		4.
		5.
	
	
	
	
	
	*/





	while (ModeHandle->getMode() == 3) {
		int angle = SensorHandle->getEyeAngle();
		int posX = ServoState::defaultX + angle;

		//SerialHandle->write_port(ServoHandle->setX(posX));
		Sleep(50);
	}


}


/***********************************************************
Fan Services
***********************************************************/
void ServoSweep() {
	int minX = ServoState::minX;
	int maxX = ServoState::maxX;
	int minY = ServoState::minY;
	int maxY = ServoState::maxY;
	int speed = 6;
	int delay = 1000;

	SerialHandle->write_port(ServoHandle->reset());
	Sleep(500);
	while (1) {
		cout << "outer loop" << endl;
		Sleep(delay);
		while (ServoHandle->getX() > minX) {
			cout << "pan left:" << ServoHandle->getX() << endl;
			SerialHandle->write_port(ServoHandle->pan(-speed));
			Sleep(delay);
		}
		while (ServoHandle->getX() < maxX) {
			cout << "pan right:" << ServoHandle->getX() << endl;
			SerialHandle->write_port(ServoHandle->pan(speed));
			Sleep(delay);
		}
	}
}

/***********************************************************
Mode Management
***********************************************************/
void modeControl() {
	int started = 0;

	while (1) {
		//update mode
		if ((GetAsyncKeyState(0x09) < 0) || (SensorHandle->getEEGAttentionChange() == 1)) {//Toggle Mode: (tab) || EEG_Attention || gamepadY
			ModeHandle->toggleMode();
			SensorHandle->setEEGAttentionChange(0);
			Sleep(150);//prevent fast switching
		}else if (GetAsyncKeyState(0x31) < 0 || GamepadHandle.a_button_pressed) {//num key 1 || gamepadA
			ModeHandle->setMode(1);
		}
		else if (GetAsyncKeyState(0x32) < 0 || GamepadHandle.b_button_pressed)//num key 2 || gamepadB
		{
			ModeHandle->setMode(2);
		}
		else if (GetAsyncKeyState(0x33) < 0 || GamepadHandle.y_button_pressed) {//num key 3 || gamepadY
			ModeHandle->setMode(3);
		}

		//Mode Switching
		if (!started || (ModeHandle->getChange() && ModeHandle->getMode() == 1)) {
			/***********************MODE1: Real-time keyboard control*************************/
			Console::WriteLine("INFO: Enter MODE1: real-time keyboard control");
			ThreadStart^ mode1MotorThreadDelegate = gcnew ThreadStart(&Mode1MotorControl);
			Thread^ mode1MotorThread = gcnew Thread(mode1MotorThreadDelegate);
			mode1MotorThread->Start();
			ThreadStart^ mode1ServoThreadDelegate = gcnew ThreadStart(&Mode1ServoControl);
			Thread^ mode1ServoThread = gcnew Thread(mode1ServoThreadDelegate);
			mode1ServoThread->Start();
			ModeHandle->setChange(0);
			started = 1;
		}
		else if (ModeHandle->getChange() && ModeHandle->getMode() == 2) {
			/**********************MODE2: Real-time handsfree control*************************/
			Console::WriteLine("INFO: Enter MODE2: real-time handsfree control");
			//ThreadStart^ mode2RealTimeThreadDelegate = gcnew ThreadStart(&Mode2RealTimeControl);
			//Thread^ mode2RealTimeThread = gcnew Thread(mode2RealTimeThreadDelegate);
			//mode2RealTimeThread->Start();
			ModeHandle->setChange(0);
		}
		else if (ModeHandle->getChange() && ModeHandle->getMode() == 3) {
			/**********************MODE3: Assited handsfree control*************************/
			Console::WriteLine("INFO: Enter MODE3: assited hands-free control");
			ThreadStart^ mode3AssistedThreadDelegate = gcnew ThreadStart(&Mode3AssistedControl);
			Thread^ mode3AssistedThread = gcnew Thread(mode3AssistedThreadDelegate);
			//mode3AssistedThread->Start();
			ModeHandle->setChange(0);
		}

		Sleep(100);
	}

}


/***********************************************************
Connection Management
***********************************************************/
void serialConnection() {
	Sleep(1000);

	Console::WriteLine("INFO: Attempt Connecting to Serial Port");
	if (SerialHandle->open_port()) {
		//SerialHandle->setup_params();
		//SerialHandle->timeout();
		Beep(750, 1000);
	}

	while (1) {
		if (GetAsyncKeyState(0x24) < 0) { //Home
			if (SerialHandle->isConnected()) {
				Console::WriteLine("INFO: Attempt Closing Serial Port");
				SerialHandle->close_port();
				Beep(250, 1000);
			}
			else {
				Console::WriteLine("INFO: Attempt Connecting to Serial Port");
				if (SerialHandle->open_port()) {
					//SerialHandle->setup_params();
					//SerialHandle->timeout();
					Beep(750, 1000);
				}
			}
			Sleep(150);
		}
		Sleep(100);
	}
}

void UDPReceiveConnection() {
	Sleep(8000);
	Console::WriteLine("INFO: Attempt Connecting to UDP Port");

	//receive from remote client
	UdpClient^ udpClientReceive = gcnew UdpClient(UDPPORT);
	IPEndPoint^ ipEndPoint = gcnew IPEndPoint(IPAddress::Any, UDPPORT);
	array<Byte> ^ receivedBytes = gcnew array<Byte>(1024);

	while (1) {
		receivedBytes = udpClientReceive->Receive(ipEndPoint);
		String^ receivedStr = Encoding::ASCII->GetString(receivedBytes, 0, receivedBytes->Length);
		msclr::interop::marshal_context context;
		std::string receivedCmd = context.marshal_as<std::string>(receivedStr);
		
		if (receivedBytes){ 
			//Console::WriteLine(String::Concat("This message was sent from ", ipEndPoint->Address->ToString(), " on their port number ", ipEndPoint->Port.ToString()));	
			Console::WriteLine(receivedStr); 	
			SensorHandle->setState(receivedCmd);
			Sleep(10);
		}
		else {
			Console::WriteLine("Warning: data not recognized");
			Sleep(500);
		}
		Sleep(100);

	}
}

void UDPSendConnection() {

	UdpClient^ udpClientSend = gcnew UdpClient;
	//udpClientSend->Connect("138.51.234.242", 2); //BA LAB
	//udpClientSend->Connect("138.51.224.231", 2);
	udpClientSend->Connect("100.64.223.168", 2);
	//udpClientSend->Connect("localhost",2);
	while (1) { 
		//trigger if a change in mode
		if ((GetAsyncKeyState(0x09) < 0) || (SensorHandle->getEEGAttentionChange() == 1)) {
			Sleep(300);
			array<Byte>^sendBytes = Encoding::ASCII->GetBytes("M" + ModeHandle->getMode() + ";");
			udpClientSend->Send(sendBytes, sendBytes->Length);
			//Console::WriteLine("Mode Change Sent");
		}
		Sleep(100);
	}
	
}

/***********************************************************
Xinput support (gamepad)
***********************************************************/
void gamepadControl() {
	while (1) {
		GamepadHandle.update();

		//vibrate on mode change
		if (GamepadHandle.a_button_pressed || GamepadHandle.b_button_pressed || GamepadHandle.x_button_pressed || GamepadHandle.y_button_pressed)
			GamepadHandle.vibrate(2);



		Sleep(100);
	}
}


/***********************************************************
Main
***********************************************************/

int main(int argc, char **argv) {
	
	/**********************Exception Handling*************************/
	//Console Events Handling
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

	/**********************Connection management*************************/
	//serial connection management
	ThreadStart^ serialThreadDelegate = gcnew ThreadStart(&serialConnection);
	Thread^ serialThread = gcnew Thread(serialThreadDelegate);
	serialThread->Start();

	//udp connection management (receive)
	ThreadStart^ UDPReceiveThreadDelegate = gcnew ThreadStart(&UDPReceiveConnection);
	Thread^ UDPReceiveThread = gcnew Thread(UDPReceiveThreadDelegate);
	UDPReceiveThread->Start();

	//udp connection management (send)
	ThreadStart^ UDPSendThreadDelegate = gcnew ThreadStart(&UDPSendConnection);
	Thread^ UDPSendThread = gcnew Thread(UDPSendThreadDelegate);
	UDPSendThread->Start();
	/**********************Mode management*************************/
	//mode management
	ThreadStart^ modeThreadDelegate = gcnew ThreadStart(&modeControl);
	Thread^ modeThread = gcnew Thread(modeThreadDelegate);
	modeThread->Start();
	/**********************Xinput Support*************************/
	//xbox controller
	ThreadStart^ gamepadThreadDelegate = gcnew ThreadStart(&gamepadControl);
	Thread^ gamepadThread = gcnew Thread(gamepadThreadDelegate);
	gamepadThread->Start();

	while (1){ }

	//End Operation
	Console::WriteLine("Exit Happily");

	return 0;



}

