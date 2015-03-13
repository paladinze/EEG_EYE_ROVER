#include "GameController.h"


GameController::GameController()
{
	//clear mem
	ZeroMemory(&state, sizeof(XINPUT_STATE));
}


GameController::~GameController()
{
}

void GameController::vibrate(float duration = 2) 
{
	vibration.wLeftMotorSpeed = 32000; // use any value between 0-65535 here
	vibration.wRightMotorSpeed = 32000; // use any value between 0-65535 here
	leftMotorTimer = duration;
	rightMotorTimer = duration;
	XInputSetState(controller_id, &vibration);
}

void GameController::vibrate(float duration = 2, WORD speedLeft = 32000, WORD speedRight = 16000) {
	vibration.wLeftMotorSpeed = speedLeft; // use any value between 0-65535 here
	vibration.wRightMotorSpeed = speedRight; // use any value between 0-65535 here
	leftMotorTimer = duration;
	rightMotorTimer = duration;
	XInputSetState(controller_id, &vibration);
}


void GameController::setControllerID(int ID){
	controller_id = ID;
}

void GameController::setDeadzone(float val){
	deadzone = val;
}

float GameController::setJoystick(float val) {
	float normVal = fmaxf(-1, val / 32767);
	float trimVal = (abs(normVal) < deadzone ? 0 : (abs(normVal) - deadzone) * (normVal / abs(normVal)));
	if (deadzone > 0) trimVal /= 1 - deadzone;
	return trimVal;
}


void GameController::update()
{

	//confirm connection and update raw data
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	isConnected = (XInputGetState(controller_id, &state) == ERROR_SUCCESS);

	//right gamepad
	a_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0);
	b_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0);
	x_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0);
	y_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0);
	
	//left gamepad
	upDpad_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0);
	downDpad_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0);
	leftDpad_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0);
	rightDpad_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0);

	//joysticks
	lx_thumb = setJoystick((float)state.Gamepad.sThumbLX);
	ly_thumb = setJoystick((float)state.Gamepad.sThumbLY);
	rx_thumb = setJoystick((float)state.Gamepad.sThumbRX);
	ry_thumb = setJoystick((float)state.Gamepad.sThumbRY);

	//triggers
	left_trigger = (float)state.Gamepad.bLeftTrigger / 255;
	right_trigger = (float)state.Gamepad.bRightTrigger / 255;

	//update vibration timer
	if (rightMotorTimer > 0)
	{
		rightMotorTimer -= timerSpeed;
	}
	else if (rightMotorTimer < 1)
	{
		vibration.wRightMotorSpeed = 0;
	}

	if (leftMotorTimer > 0)
	{
		leftMotorTimer -= timerSpeed;
	}
	else if (leftMotorTimer < 1)
	{
		vibration.wLeftMotorSpeed = 0;
	}

	//update vibration state
	XInputSetState(controller_id, &vibration);

}

