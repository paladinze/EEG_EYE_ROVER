#pragma once

#include <Windows.h>
#include <Xinput.h>
#include <math.h>
#include <iostream>
using namespace System;
using namespace std;


class GameController
{

public:
	//raw data
	XINPUT_STATE state;
	XINPUT_VIBRATION vibration;

	//connection
	int controller_id = 0; // max 4 controllers for pc
	bool isConnected = 0;
	
	//right gamepad
	bool a_button_pressed = 0;
	bool b_button_pressed = 0;
	bool x_button_pressed = 0;
	bool y_button_pressed = 0;

	//left gamepad
	bool upDpad_button_pressed = 0;
	bool downDpad_button_pressed = 0;
	bool leftDpad_button_pressed = 0;
	bool rightDpad_button_pressed = 0;

	//joysticks
	float lx_thumb = 0;
	float ly_thumb = 0;
	float rx_thumb = 0;
	float ry_thumb = 0;

	//trigger
	float left_trigger = 0;
	float right_trigger = 0;

	//vibration timer
	float leftMotorTimer = 0;
	float rightMotorTimer = 0;

	//params
	float deadzone = 0.2;
	float timerSpeed = 1;


	GameController();
	~GameController();

	void update();
	void vibrate(float,WORD, WORD);
	void vibrate(float);
	float setJoystick(float);
	void setControllerID(int);
	void setDeadzone(float);
};

