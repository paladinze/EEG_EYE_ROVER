#pragma once
#include <string>
#include <regex>
#include <iostream>
#include <Windows.h>
#include <vector>

class SensorComm
{
private:

	//State for Mode2
	int moveObserveState = 0; // 0: observe; 1: move;

	//EEG
	int eegBlink = 0;

	int eegTeethPrev = 0;
	int eegTeethCurr = 0;
	int eegTeethChange = 0;

	int eegEyebrowChange = 0;
	int eegEyebrowPrev = 0;
	int eegEyebrowCurr = 0;

	int eegAttentionChange = 0;
	int eegAttentionPrev = 0;
	int eegAttentionCurr = 0;
	

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
	int getMoveObserveState();

	int getEEGTeeth();
	int getEEGTeethChange();

	int getEEGBlink();

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

