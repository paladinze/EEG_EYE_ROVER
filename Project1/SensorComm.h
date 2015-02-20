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

	int eegTeethChange = 0;
	int eegAttentionChange = 0;
	int eegEyebrowChange = 0;

	//Eye tracker
	int eyeActive = 0; //0,1
	int eyeActiveChange = 0;

	std::string eyeQuadrant = "n"; //w, s, a, d, n(center)
	int eyeCoordinate[2]; //screen coordinates
	int eyeAngle = 0; //angle with respect the central axis
	
public:
	SensorComm();
	~SensorComm();



	//get
	int getEEGTeeth();
	int getEEGTeethChange();

	int getEEGAttention();
	int getEEGAttentionChange();

	int getEEGEyebrow();
	int getEEGEyebrowChange();

	int getEyeActive();
	int getEyeActiveChange();

	std::string getEyeQuadrant();
	int* getEyeCoordinate();
	int getEyeAngle();

	//set
	void setEEGAttention(int);
	void setEEGAttentionChange(int);

	void setEEGTeeth(int);
	void setEEGTeethChange(int);

	void setEEGEyebrow(int);
	void setEEGEyebrowChange(int);

	void setEyeQuadrant(std::string);

	void setState(std::string);
};

