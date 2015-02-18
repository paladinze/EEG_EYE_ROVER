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
	while (ModeHandle->getMode() == 1) {

		if (GetAsyncKeyState(VK_UP) <0) { //arrow_up
			Console::WriteLine("\t\tCar Forward");
			SerialHandle->write_port(MotorHandle->forward());
		}
		else if (GetAsyncKeyState(VK_DOWN) <0) { //arrow_down
			Console::WriteLine("\t\tCar Backward");
			SerialHandle->write_port(MotorHandle->backward());
		}
		else if (GetAsyncKeyState(VK_LEFT) <0) { //arrow_left
			Console::WriteLine("\t\tCar Left");
			SerialHandle->write_port(MotorHandle->left());
		}
		else if (GetAsyncKeyState(VK_RIGHT) <0) { //arrow_right
			Console::WriteLine("\t\tCar Right");
			SerialHandle->write_port(MotorHandle->right());
		}
		Sleep(10); // can't be slower than this
	}
}

void Mode1ServoControl() {
	//Console::WriteLine("\tEnable mode1 servo control");
	while (ModeHandle->getMode() == 1) {
		if (GetAsyncKeyState(0x52) <0) {//Reset (R)
			SerialHandle->write_port(ServoHandle->reset());
			Console::WriteLine("\t\tCAM Reset");
		} 
		else if (GetAsyncKeyState(0x47) <0) {//sweep (G)
			SerialHandle->write_port(ServoHandle->sweep());
			Console::WriteLine("\t\tCAM Sweep");
		}
		else if (GetAsyncKeyState(0x57) <0) {//W
			SerialHandle->write_port(ServoHandle->setY(60));
			//SerialHandle->write_port(ServoHandle->tilt(+40));
			Console::WriteLine("\t\tCAM UP");
		}
		else if (GetAsyncKeyState(0x53) <0) {//S
			SerialHandle->write_port(ServoHandle->setY(20));
			//SerialHandle->write_port(ServoHandle->tilt(-10));
			Console::WriteLine("\t\tCAM DOWN");
		}
		else if (GetAsyncKeyState(0x41) <0) {//A
			SerialHandle->write_port(ServoHandle->setX(60));
			//SerialHandle->write_port(ServoHandle->pan(-35));
			Console::WriteLine("\t\tCAM LEFT");
		}
		else if (GetAsyncKeyState(0x44) <0) {//D
			SerialHandle->write_port(ServoHandle->setX(130));
			//SerialHandle->write_port(ServoHandle->pan(35));
			Console::WriteLine("\t\tCAM RIGHT");
		}
		Sleep(120);
	}

}


/***********************************************************
*MODE2:real-time handsfree
***********************************************************/
void Mode2RealTimeControl() {
	Console::WriteLine("\tEnable mode2 eye control");
	Console::WriteLine("\tEnable mode2 EEG control");

	while (1) {

		if (SensorHandle->getEEGTeeth() == 1) { 
			//Console::WriteLine("\tMovement State");

			if (SensorHandle->getEyeQuadrant() == "a") {//A
				for (int i = 0; i < 1; i++) {
					SerialHandle->write_port(MotorHandle->left());
				}
				//Console::WriteLine("Eye LEFT => Car left");
			}
			else if (SensorHandle->getEyeQuadrant() == "d") {//D
				for (int i = 0; i < 1; i++) {
					SerialHandle->write_port(MotorHandle->right());
				}
				//Console::WriteLine("Eye RIGHT => Car right");
			}
			else {
				//Console::WriteLine("\t\tCar Forward");
				SerialHandle->write_port(MotorHandle->forward());
			}

			Sleep(50); // can't be slower than this
		}
		else if (SensorHandle->getEEGTeeth() == 0) {
			//Console::WriteLine("\tObservation State"); 
			if (SensorHandle->getEyeActive() == 0) {
				//Console::WriteLine("Eye trakcer inactive");
			}
			else {
				if (SensorHandle->getEEGEyebrow() == 1) {
					//SerialHandle->write_port(ServoHandle->sweep());
					Console::WriteLine("Motor sweep");
					Sleep(3000);
				}
				else if (SensorHandle->getEyeQuadrant() == "w") {//W
					if (ServoHandle->getY() > ServoHandle->defaultY) {
					}
					else {
						SerialHandle->write_port(ServoHandle->tilt(+40));
						Console::WriteLine("CAM UP");
					}
				}
				else if (SensorHandle->getEyeQuadrant() == "s") {//S
					if (ServoHandle->getY() < ServoHandle->defaultY) {
					}
					else {
						SerialHandle->write_port(ServoHandle->tilt(-10));
						Console::WriteLine("CAM DOWN");
					}
				}
				else if (SensorHandle->getEyeQuadrant() == "a") {//A
					if (ServoHandle->getX() < ServoHandle->defaultX) {
						Console::WriteLine("CAM STATIC");
					}
					else {
						SerialHandle->write_port(ServoHandle->pan(-35));
						Console::WriteLine("CAM LEFT");
					}
				}
				else if (SensorHandle->getEyeQuadrant() == "d") {//D
					if (ServoHandle->getX() > ServoHandle->defaultX) {
						Console::WriteLine("CAM STATIC");
					}
					else {
						SerialHandle->write_port(ServoHandle->pan(+35));
						Console::WriteLine("CAM RIGHT");
					}
				}
				else if (SensorHandle->getEyeQuadrant() == "n") {//Center
					//SerialHandle->write_port(ServoHandle->reset());
					Console::WriteLine("CAM Static");
				}
				Sleep(300);
			}
		}
		

	}	
}


/***********************************************************
*MODE3:assisted handsfree
***********************************************************/
void Mode3AssistedControl() {
	Console::WriteLine("welcome to mode3! We are too lazy to implement it");
	while (1) {
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
	int speed = 5;
	int delay = 100;

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

	bool started = false;
	while (1) {
		//Initalization
		if (!started) {
			//Default: MODE1: Manual Mode (keyboard)
			Console::WriteLine("INFO: Enter MODE1: Manual Mode (keyboard)");
			ThreadStart^ mode1MotorThreadDelegate = gcnew ThreadStart(&Mode1MotorControl);
			Thread^ mode1MotorThread = gcnew Thread(mode1MotorThreadDelegate);
			mode1MotorThread->Start();

			ThreadStart^ mode1ServoThreadDelegate = gcnew ThreadStart(&Mode1ServoControl);
			Thread^ mode1ServoThread = gcnew Thread(mode1ServoThreadDelegate);
			mode1ServoThread->Start();

			started = true;
		}

		//Mode Switching
		if (( GetAsyncKeyState(0x09) < 0) || (SensorHandle->getEEGAttention() == 1)) {//Toggle Mode: (tab) || EEG_Attention
			ModeHandle->toggleMode();
			SensorHandle->setEEGAttention(0);
			Sleep(150);

			if (ModeHandle->getMode() == 1) {
				/***********************MODE1: Real-time keyboard control*************************/
				Console::WriteLine("INFO: Enter MODE1: real-time keyboard control");
				ThreadStart^ mode1MotorThreadDelegate = gcnew ThreadStart(&Mode1MotorControl);
				Thread^ mode1MotorThread = gcnew Thread(mode1MotorThreadDelegate);
				mode1MotorThread->Start();

				ThreadStart^ mode1ServoThreadDelegate = gcnew ThreadStart(&Mode1ServoControl);
				Thread^ mode1ServoThread = gcnew Thread(mode1ServoThreadDelegate);
				mode1ServoThread->Start();
			}
			else if (ModeHandle->getMode() == 2) {
				/**********************MODE2: Real-time handsfree control*************************/
				Console::WriteLine("INFO: Enter MODE2: real-time handsfree control");
				ThreadStart^ mode2RealTimeThreadDelegate = gcnew ThreadStart(&Mode2RealTimeControl);
				Thread^ mode2RealTimeThread = gcnew Thread(mode2RealTimeThreadDelegate);
				mode2RealTimeThread->Start();
			}
			else if (ModeHandle->getMode() == 3) {
				/**********************MODE3: Assited handsfree control*************************/
				Console::WriteLine("INFO: Enter MODE3: assited hands-free control");
				ThreadStart^ mode3AssistedThreadDelegate = gcnew ThreadStart(&Mode3AssistedControl);
				Thread^ mode3AssistedThread = gcnew Thread(mode3AssistedThreadDelegate);
				mode3AssistedThread->Start();;
			}
			else {
				Console::WriteLine("invalid mode");
			}
		}
	}

}


/***********************************************************
Connection Management
***********************************************************/
void serialConnection() {
	Sleep(1000);

	Console::WriteLine("INFO: Attempt Connecting to Serial Port");
	if (SerialHandle->open_port()) {
		SerialHandle->setup_params();
		SerialHandle->timeout();
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
					SerialHandle->setup_params();
					SerialHandle->timeout();
					Beep(750, 1000);
				}
			}
			Sleep(150);
		}
	}
}

void UDPReceiveConnection() {
	Sleep(3000);
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
		}
		else {
			Console::WriteLine("Warning: data not recognized");
			Sleep(500);
		}

	}
}

void UDPSendConnection() {

	UdpClient^ udpClientSend = gcnew UdpClient;
	udpClientSend->Connect("138.51.234.242", 2); //BA LAB
	//udpClientSend->Connect("localhost",2);
	while (1) { 
		//trigger if a change in mode
		if ((GetAsyncKeyState(0x09) < 0) || (SensorHandle->getEEGAttention() == 1)) {
			Sleep(300);
			array<Byte>^sendBytes = Encoding::ASCII->GetBytes("M" + ModeHandle->getMode() + ";");
			udpClientSend->Send(sendBytes, sendBytes->Length);
			//Console::WriteLine("Mode Change Sent");
		}
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

	while (1){ }

	//End Operation
	Console::WriteLine("Exit Happily");

	return 0;



}

