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
#include <string>
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
*MODE1: Manual Mode (keyboard)
*	key press control
*MODE2: Manual Mode (Hands-free)
*	real time control
*MODE3: Semi-Autonoumous mode (Hands-free)
*	point and go
*MODE4: Autonoumous mode (GPS)
*/


/***********************************************************
*MODE1: Manual Mode (keyboard)
***********************************************************/
void MotorControl() {
	short esc = 0;
	Console::WriteLine("Enable motor control");

	while (ModeHandle->getMode() == 1) {
		//terminate condition
		esc = GetAsyncKeyState(VK_ESCAPE);

		//Control signal to communication signal mapping
		if (GetAsyncKeyState(VK_UP) <0) {
			Console::WriteLine("Car Forward");
			SerialHandle->write_port(MotorHandle->forward());
		}
		else if (GetAsyncKeyState(VK_DOWN) <0) {
			Console::WriteLine("Car Backward");
			SerialHandle->write_port(MotorHandle->backward());
		}
		else if (GetAsyncKeyState(VK_LEFT) <0) {
			Console::WriteLine("Car Left");
			SerialHandle->write_port(MotorHandle->left());
		}
		else if (GetAsyncKeyState(VK_RIGHT) <0) {
			Console::WriteLine("Car Right");
			SerialHandle->write_port(MotorHandle->right());
		}
		Sleep(10); // can't be slower than this
	}
	
}

void ServoControl() {
	short esc = 0;
	Console::WriteLine("Enable servo control");
	while (ModeHandle->getMode() == 1) {
		//terminate condition
		esc = GetAsyncKeyState(VK_ESCAPE);

		if (GetAsyncKeyState(0x52) <0) {//Reset (R)
			SerialHandle->write_port(ServoHandle->reset());
			Console::WriteLine("CAM Reset");
		} 
		else if (GetAsyncKeyState(0x47) <0) {//sweep (G)
			SerialHandle->write_port(ServoHandle->sweep());
			Console::WriteLine("CAM Sweep");
		}
		else if (GetAsyncKeyState(0x57) <0) {//W
			SerialHandle->write_port(ServoHandle->setY(60));
			Console::WriteLine("CAM UP");
		}
		else if (GetAsyncKeyState(0x53) <0) {//S
			SerialHandle->write_port(ServoHandle->setY(20));
			Console::WriteLine("CAM DOWN");
		}
		else if (GetAsyncKeyState(0x41) <0) {//A
			for (int i = 0; i < 1; i++) {
				SerialHandle->write_port(ServoHandle->setX(60));
			}
			Console::WriteLine("CAM LEFT");
		}
		else if (GetAsyncKeyState(0x44) <0) {//D
			for (int i = 0; i < 1; i++) {
				SerialHandle->write_port(ServoHandle->setX(130));
			}
			Console::WriteLine("CAM RIGHT");
		}
		Sleep(120);
	}

}





/***********************************************************
*MODE2: Manual Mode (Hands-free)
***********************************************************/
void EegControl() {
	Console::WriteLine("Enable eeg control");

}


void EyeControl() {
	Console::WriteLine("Enable eye control");

/*	//send to remote client
	UdpClient^ RemoteClient = gcnew UdpClient;
	try {
		RemoteClient->Connect("localhost",11001);
		array<Byte>^sendBytes = Encoding::ASCII->GetBytes("Is anybody there?");
		RemoteClient->Send
	}

*/
	//receive from remote client
	UdpClient^ udpClient = gcnew UdpClient(UDPPORT);
	IPEndPoint^ ipEndPoint = gcnew IPEndPoint(IPAddress::Any, UDPPORT);
	array<Byte> ^ receivedBytes = gcnew array<Byte>(1024);

	while (1) {
		receivedBytes = udpClient->Receive(ipEndPoint);
		String^ receivedStr = Encoding::ASCII->GetString(receivedBytes, 0, receivedBytes->Length);
		msclr::interop::marshal_context context;
		std::string receivedCmd = context.marshal_as<std::string>(receivedStr);

		Console::WriteLine(String::Concat("This message was sent from ", ipEndPoint->Address->ToString(), " on their port number ", ipEndPoint->Port.ToString()));
		


		Console::WriteLine(receivedStr);

		if (receivedCmd == "Quadrant Up") {//W
			//SerialHandle->write_port('Mw');
			Console::WriteLine("CAM UP");
		}
		else if (receivedCmd == "Quadrant Down") {//S
			//SerialHandle->write_port('Ms');
			Console::WriteLine("CAM DOWN");
		}
		else if (receivedCmd == "Quadrant Left") {//A
			for (int i = 0; i < 1; i++) {
				//SerialHandle->write_port('Ma');
			}
			Console::WriteLine("CAM LEFT");
		}
		else if (receivedCmd == "Quadrant Right") {//D
			for (int i = 0; i < 1; i++) {
				//SerialHandle->write_port('Md');
			}
			Console::WriteLine("CAM RIGHT");
		}
		else if (receivedCmd == "Gaze point lost") {
			Console::WriteLine("Gaze Data Lost");
		}

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

		if (!started) {
			//Default: MODE1: Manual Mode (keyboard)
			Console::WriteLine("INFO: Enter MODE1: Manual Mode (keyboard)");
			ThreadStart^ motorThreadDelegate = gcnew ThreadStart(&MotorControl);
			Thread^ motorThread = gcnew Thread(motorThreadDelegate);
			motorThread->Start();

			ThreadStart^ servoThreadDelegate = gcnew ThreadStart(&ServoControl);
			Thread^ servoThread = gcnew Thread(servoThreadDelegate);
			servoThread->Start();

			started = true;
		}


		if (GetAsyncKeyState(0x09) <0) {//Toggle (tab)
			ModeHandle->toggleMode();
			Sleep(150);

			if (ModeHandle->getMode() == 1) {
				/***********************MODE1: Manual Mode (keyboard)*************************/
				//Manual Control (Keyboard)
				Console::WriteLine("INFO: Enter MODE1: Manual Mode (keyboard)");
				ThreadStart^ motorThreadDelegate = gcnew ThreadStart(&MotorControl);
				Thread^ motorThread = gcnew Thread(motorThreadDelegate);
				motorThread->Start();

				ThreadStart^ servoThreadDelegate = gcnew ThreadStart(&ServoControl);
				Thread^ servoThread = gcnew Thread(servoThreadDelegate);
				servoThread->Start();
			}
			else if (ModeHandle->getMode() == 2) {
				/**********************MODE2: Manual Mode (Hands-free)*************************/
				//Real-time control
				Console::WriteLine("INFO: Enter MODE2: Manual Mode (Hands-free)");
				ThreadStart^ eyeThreadDelegate = gcnew ThreadStart(&EyeControl);
				Thread^ eyeThread = gcnew Thread(eyeThreadDelegate);
				eyeThread->Start();
			}
			else if (ModeHandle->getMode() == 3) {
				/**********************MODE3: Semi-Autonoumous mode (Hands-free)*************************/
				//Point and Go!
				Console::WriteLine("INFO: Enter MODE3: Semi-Autonoumous mode (Hands-free)");
				ThreadStart^ sweepThreadDelegate = gcnew ThreadStart(&ServoSweep);
				Thread^ sweepThread = gcnew Thread(sweepThreadDelegate);
				sweepThread->Start();;
			}


		}
	}

}

//REQ:K (disable)
//REQ:Q (quadrant)
/***********************************************************
Connection Management
***********************************************************/
void serialControl() {
	//Connect to Serial Port
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

/***********************************************************
Main
***********************************************************/

int main(int argc, char **argv) {
	

	/**********************Exception Handling*************************/
	//Console Events Handling
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);

	/**********************Connection Initiation*************************/


	//serial connection management
	ThreadStart^ serialThreadDelegate = gcnew ThreadStart(&serialControl);
	Thread^ serialThread = gcnew Thread(serialThreadDelegate);
	serialThread->Start();

	//mode management
	ThreadStart^ modeThreadDelegate = gcnew ThreadStart(&modeControl);
	Thread^ modeThread = gcnew Thread(modeThreadDelegate);
	modeThread->Start();


	while (1){}


	//End Operation
	Console::WriteLine("Exit Happily");

	return 0;



}

