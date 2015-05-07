/**********************************************************
*Ground Rover Communication Module
*SubModules
*	1.Eyetracker (UDP receiving end)
*	2.Serial Communication
*Author: Ze Cheng
*Liscence: Public Domain


/***********************************************************
Modes Description & Communication protocl
***********************************************************/
//TODO:add description

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
BtSerial* SerialHandle = new BtSerial("\\\\.\\COM14");
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
			else if (GamepadHandle.x_button_pressed){
				Console::WriteLine("\t\tCar Assisted");
				SerialHandle->write_port(MotorHandle->assisted(),1);
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
			else if (GetAsyncKeyState(0x58) < 0) { //x: sonar assisted
				Console::WriteLine("\t\tCar Sonar Assisted");
				SerialHandle->write_port(MotorHandle->assisted(), 1);
				Sleep(motorDelay);
			}
		}
		Sleep(100);
	}
}

void Mode1ServoControl() {
	Console::WriteLine("\tEnable mode1 servo control");
	int camDelay = 0;
	float joystickThres = 0;
	float maxJoySpeed = 5;
	float maxKeySpeed = 5;
	float realSpeed = 0;
	while (ModeHandle->getMode() == 1) {
		
		if (GamepadHandle.isConnected) {
			//Servo: gamepad left joystick
			if (GamepadHandle.right_trigger == 1) {//Reset (R)
				Console::WriteLine("\t\tCAM Reset");
				SerialHandle->write_port(ServoHandle->reset());
				Sleep(camDelay);
			}
			else if (GamepadHandle.left_trigger == 1) {//sweep (G)
				Console::WriteLine("\t\tCAM Sweep");
				SerialHandle->write_port(ServoHandle->sweep());
				Sleep(camDelay);
			}
			else if (GamepadHandle.ly_thumb > joystickThres) { //arrow_up
				Console::WriteLine("CAM UP");
				realSpeed = maxJoySpeed * GamepadHandle.ly_thumb;
				SerialHandle->write_port(ServoHandle->tilt((int)round(realSpeed)));
			}
			else if (GamepadHandle.ly_thumb < -joystickThres) { //arrow_down
				Console::WriteLine("CAM DOWN");
				realSpeed = maxJoySpeed * GamepadHandle.ly_thumb;
				SerialHandle->write_port(ServoHandle->tilt((int)round(realSpeed)));
			}
			else if (GamepadHandle.lx_thumb < -joystickThres) { //arrow_left
				Console::WriteLine("CAM LEFT");
				realSpeed = maxJoySpeed * GamepadHandle.lx_thumb;
				SerialHandle->write_port(ServoHandle->pan((int)round(realSpeed)));
			}
			else if (GamepadHandle.lx_thumb > joystickThres) { //arrow_right
				Console::WriteLine("CAM RIGHT");
				realSpeed = maxJoySpeed * GamepadHandle.lx_thumb;
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
			else if (GetAsyncKeyState(0x53) <0) {//cam down
				SerialHandle->write_port(ServoHandle->tilt(maxKeySpeed));
				Sleep(camDelay);
				Console::WriteLine("CAM DOWN");
			}
			else if (GetAsyncKeyState(0x57) < 0) {//cam up
				SerialHandle->write_port(ServoHandle->tilt(-maxKeySpeed));
				Sleep(camDelay);
				Console::WriteLine("CAM UP");
			}
			else if (GetAsyncKeyState(0x41) < 0) {//cam left
				SerialHandle->write_port(ServoHandle->pan(-maxKeySpeed));
				Sleep(camDelay);
				Console::WriteLine("CAM LEFT");
			}
			else if (GetAsyncKeyState(0x44) <0) {//cam right
				SerialHandle->write_port(ServoHandle->pan(maxKeySpeed));
				Sleep(camDelay);
				Console::WriteLine("CAM RIGHT");
			}

		}

		Sleep(100);
	}
}



/***********************************************************
*MODE2:real-time handsfree
***********************************************************/

void Mode2RealTimeControl(){
	Console::WriteLine("\Entering Mode2: real-time control");
	Console::WriteLine("\tEnable mode2 eye control");
	Console::WriteLine("\tEnable mode2 EEG control");
	Console::WriteLine("\tEntering defualt state: observation state");

	while (ModeHandle->getMode() == 2) {
		/******************************handle mode change: modeX => mode2******************************/
		if (SensorHandle->getGyroYChange() == 1 || GetAsyncKeyState(0x54) < 0 || GamepadHandle.x_button_pressed) {//Key T or gamepadX or GyroY

			//Reset camera to look down
			SerialHandle->write_port(ServoHandle->setX(ServoHandle->initX));
			SerialHandle->write_port(ServoHandle->setY(ServoHandle->maxY));
			Sleep(300);
			Console::WriteLine("\t\tCAM Reset");

			//Print current state
			if (SensorHandle->getMoveObserveState() == 0){
				Console::WriteLine("\tEntering observation state");
			}
			else {
				Console::WriteLine("\tEntering movement state");
			}
			SensorHandle->setGyroYChange(0);
		}

		/******************************handle state: movement state******************************/ //(motor and servo needs adjust)
		if (SensorHandle->getMoveObserveState() == 1) { 
			if (SensorHandle->getEyeQuadrant() == "a") { //motor left
				Console::WriteLine("\t\tCar Left");
				int motorDelay = 100;
				int motorSpeed = 10;
				SerialHandle->write_port(MotorHandle->left(),motorSpeed);
				SensorHandle->setEyeQuadrant("n");
				Sleep(motorDelay);
			}
			else if (SensorHandle->getEyeQuadrant() == "d") {//motor right
				Console::WriteLine("\t\tCar Right");
				int motorDelay = 100;
				int motorSpeed = 10;
				SerialHandle->write_port(MotorHandle->right(),motorSpeed);
				SensorHandle->setEyeQuadrant("n");
				Sleep(motorDelay);
			}
			else if (SensorHandle->getEEGEyebrow() == 1) {//motor forward
				Console::WriteLine("\t\tCar Forward");
				int motorDelay = 40;
				int motorSpeed = 1;
				SerialHandle->write_port(MotorHandle->forward(), motorSpeed);
				Sleep(motorDelay);
			} 
			else if (SensorHandle->getEEGPushChange()){// motor backward
				Console::WriteLine("\t\tCar Backward");
				int motorDelay = 40;
				int motorSpeed = 1;
				SerialHandle->write_port(MotorHandle->backward(), motorSpeed);
				Sleep(motorDelay);
			}
		}

		/******************************handle state: Observation state******************************/
		else if (SensorHandle->getMoveObserveState() == 0) { 
			int camDelay = 50;
			int camSpeed = 1;
			
			if (SensorHandle->getGyroXChange() == 1) {//cam sweep: head horizontal movement
				Console::WriteLine("Camera sweep");
				SerialHandle->write_port(ServoHandle->sweep());
				SensorHandle->setGyroXChange(0);
				Sleep(4000);
			}
			else if (SensorHandle->getEyeQuadrant() == "w") {//cam up
				Console::WriteLine("CAM UP");
				SerialHandle->write_port(ServoHandle->tilt((int)round(-camSpeed)));
				Sleep(camDelay);
			}
			if (SensorHandle->getEyeQuadrant() == "s") {//cam down
				Console::WriteLine("CAM DOWN");
				SerialHandle->write_port(ServoHandle->tilt((int)round(+camSpeed)));
				Sleep(camDelay);
			}
			if (SensorHandle->getEyeQuadrant() == "a") {//cam left
				Console::WriteLine("CAM LEFT");
				SerialHandle->write_port(ServoHandle->pan((int)round(-camSpeed)));
				Sleep(camDelay);
			}
			if (SensorHandle->getEyeQuadrant() == "d") {//cam right
				Console::WriteLine("CAM RIGHT");
				SerialHandle->write_port(ServoHandle->pan((int)round(+camSpeed)));
				Sleep(camDelay);
			}
			else if (SensorHandle->getEyeQuadrant() == "n") {
			};
			Sleep(10);
		
		}

	}

}



/***********************************************************
*MODE3:assisted handsfree
***********************************************************/
void Mode3AssistedControl() {
	Console::WriteLine("\Entering Mode3: Assisted control");
	Console::WriteLine("\tEnable sonar sensor");
	Console::WriteLine("\tEnable EEG confirmation signal");
	Console::WriteLine("\tStare at an object and concentrate");


	/*procedure (basic version)
		0.Initialization
			1.camera reset to default position
		1.user:
			1.EYE observe screen
			2.EEG confirm eye gaze
			3.EYE send rotation angle
		2.Rover:
			1.receive angle
			2.rotate according to angle
			3.move forward using distance provided by sonar
	*/

	while (ModeHandle->getMode() == 3) {
		/******************************handle mode change: modeX => mode3******************************/
		if (SensorHandle->getGyroYChange() == 1 || GetAsyncKeyState(0x54) < 0 || GamepadHandle.x_button_pressed) {//Key T or gamepadX or GyroY

			//Reset camera to center
			SerialHandle->write_port(ServoHandle->setX(ServoHandle->initX));
			SerialHandle->write_port(ServoHandle->setY(ServoHandle->maxY));
			Sleep(300);
			Console::WriteLine("\t\tCAM Reset");
		}

		/******************************Cam follows eye movement******************************/
		int camDelay = 50;
		int camSpeed = 1;

		if (SensorHandle->getGyroXChange() == 1) {//cam sweep: head horizontal movement
			Console::WriteLine("Camera sweep");
			SerialHandle->write_port(ServoHandle->sweep());
			SensorHandle->setGyroXChange(0);
			Sleep(4000);
		}
		else if (SensorHandle->getEyeQuadrant() == "w") {//cam up
			Console::WriteLine("CAM UP");
			SerialHandle->write_port(ServoHandle->tilt((int)round(-camSpeed)));
			Sleep(camDelay);
		}
		if (SensorHandle->getEyeQuadrant() == "s") {//cam down
			Console::WriteLine("CAM DOWN");
			SerialHandle->write_port(ServoHandle->tilt((int)round(+camSpeed)));
			Sleep(camDelay);
		}
		if (SensorHandle->getEyeQuadrant() == "a") {//cam left
			Console::WriteLine("CAM LEFT");
			SerialHandle->write_port(ServoHandle->pan((int)round(-camSpeed)));
			Sleep(camDelay);
		}
		if (SensorHandle->getEyeQuadrant() == "d") {//cam right
			Console::WriteLine("CAM RIGHT");
			SerialHandle->write_port(ServoHandle->pan((int)round(+camSpeed)));
			Sleep(camDelay);
		}
		else if (SensorHandle->getEyeQuadrant() == "n") {
		};
		Sleep(10);


		/******************************Execute assisted manuevers******************************/
		//get EEG confirmation
		if (GetAsyncKeyState(0x58) < 0) { //(SensorHandle->getEEGEyebrowChange()) {
			//determine rotation angle
			int angleDeltaCurr = ServoHandle->getX() - ServoState::initX;  
			int rotationDuration = round(abs(angleDeltaCurr / 45.0) * 21);

			SerialHandle->write_port(ServoHandle->setX(ServoHandle->initX));

			//execute camera rotation and rover movement
			if (angleDeltaCurr > 0) {
				SerialHandle->write_port(MotorHandle->right(), rotationDuration); 
				Console::WriteLine("Assisted Rotation: Right");
				Console::WriteLine("angle: " + angleDeltaCurr);
			}
			else {
				SerialHandle->write_port(MotorHandle->left(), rotationDuration);
				Console::WriteLine("Assisted Rotation: Left");
				Console::WriteLine("angle: " + angleDeltaCurr);
			}
			Console::WriteLine("duration: " + rotationDuration);

			
			Sleep(1000);
			SerialHandle->write_port(MotorHandle->assisted(), 1); //forward with sonar assistance in firmware
			
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
		if ((GetAsyncKeyState(0x09) < 0) || (SensorHandle->getEEGAttentionChange() == 1)) {//Toggle Mode: (tab) || EEG_Attention 
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

		//update state (if in mode 2: the realtime mode)
		if ( ModeHandle->getMode()==2 && (SensorHandle->getGyroYChange() == 1 || GetAsyncKeyState(0x54) < 0 || GamepadHandle.x_button_pressed)) { // GyroY || key T || gamepadX
			SensorHandle->toggleMoveObserveState();
			Sleep(150);
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
			ThreadStart^ mode2RealTimeThreadDelegate = gcnew ThreadStart(&Mode2RealTimeControl);
			Thread^ mode2RealTimeThread = gcnew Thread(mode2RealTimeThreadDelegate);
			mode2RealTimeThread->Start();
			ModeHandle->setChange(0);
		}
		else if (ModeHandle->getChange() && ModeHandle->getMode() == 3) {
			/**********************MODE3: Assited handsfree control*************************/
			Console::WriteLine("INFO: Enter MODE3: assited hands-free control");
			ThreadStart^ mode3AssistedThreadDelegate = gcnew ThreadStart(&Mode3AssistedControl);
			Thread^ mode3AssistedThread = gcnew Thread(mode3AssistedThreadDelegate);
			mode3AssistedThread->Start();
			ModeHandle->setChange(0);
		}

		Sleep(100);
	}

}


/***********************************************************
Connection Management
***********************************************************/
int CheckChar(char inChar) {
	return ((inChar - '!') >= 0);
}

void serialConnection() {
	Sleep(1000);

	Console::WriteLine("INFO: Attempt Connecting to Serial Port");
	if (SerialHandle->open_port()) {
		//SerialHandle->setup_params();
		SerialHandle->timeout();
		Beep(750, 1000);
	}

	while (1) {
		std::string inMsg = "";
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
					SerialHandle->timeout();
					Beep(750, 1000);
				}
			}
			Sleep(150);
		}
		else {

			//cout << SerialHandle->read_port();
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
	//udpClientSend->Connect("100.64.215.76", 2);
	udpClientSend->Connect("192.168.0.108", 2);
	while (1) { 
		//trigger if a change in mode
		if ((GetAsyncKeyState(0x09) < 0) || (SensorHandle->getEEGAttentionChange() == 1)) {
			Sleep(300);
			array<Byte>^sendBytes = Encoding::ASCII->GetBytes("M" + ModeHandle->getMode() + ";");
			udpClientSend->Send(sendBytes, sendBytes->Length);
			Console::WriteLine("\t\tMode Change Sent");
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

		//vibratation: abxy button
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

