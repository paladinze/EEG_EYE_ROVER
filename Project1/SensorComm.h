#pragma once
#include <string>
#include <Windows.h>
using namespace std;

class SensorComm
{
public:
	//EEG
	string eegTeethState = "0";
	string eegAttention = "0";

	//Eye tracker
	string eyeActive = "0"; //0,1
	string eyeQuadrant = "N"; //w, s, a, d, N(neutral), 0(data lost) 
	string eyeCoordinate = "0,0"; //screen coordinates
	string eyeAngle = "0"; //angle with respect the central axis

public:
	SensorComm();
	~SensorComm();
	void setEEGTeethState();
	void setEEGAttention();
	void setEyeActive();
	void setEyeCoordinate();
	void setEyeAngle();
	void setState(string);
	void translateSensorToState(string);
};

