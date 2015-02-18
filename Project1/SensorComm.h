#pragma once
#include <string>
#include <regex>
#include <iostream>
#include <Windows.h>
#include <vector>

class SensorComm
{
private:

	//EEG
	int eegTeeth = 0;
	int eegAttention = 0;
	int eegEyebrow = 0;

	//Eye tracker
	int eyeActive = 0; //0,1
	std::string eyeQuadrant = "n"; //w, s, a, d, n(center)
	int eyeCoordinate[2]; //screen coordinates
	int eyeAngle = 0; //angle with respect the central axis
	
public:
	SensorComm();
	~SensorComm();

	//set
	void setEEGAttention(int);

	//get
	int getEEGTeeth();
	int getEEGAttention();
	int getEEGEyebrow();

	int getEyeActive();
	std::string getEyeQuadrant();
	int* getEyeCoordinate();
	int getEyeAngle();
	void setState(std::string);
};

