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

	//Gyro
	int gyXCurr = 0;
	int gyXPre = 0;
	int gyXChange = 0;

	//int 
	int gyYCurr = 0;
	int gyYPre = 0;
	int gyYChange = 0;

	//EEG
	int eegPushCurr = 0;
	int eegPushPrev = 0;
	int eegPushChange = 0;

	int eegPullCurr = 0;
	int eegPullPrev = 0;
	int eegPullChange = 0;

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

	void toggleMoveObserveState();

	//get
	int getMoveObserveState();

	int getGyroX();
	int getGyroXChange();

	int getGyroY();
	int getGyroYChange();

	int getEEGTeeth();
	int getEEGTeethChange();

	int getEEGBlink();

	int getEEGPush();
	int getEEGPushChange();

	int getEEGPull();
	int getEEGPullChange();

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
	void setMoveObserveState(int);

	void setGyroX(int);
	void setGyroXChange(int);

	void setGyroY(int);
	void setGyroYChange(int);

	void setEEGPush(int);
	void setEEGPushChange(int);

	void setEEGPull(int);
	void setEEGPullChange(int);

	void setEEGAttention(int);
	void setEEGAttentionChange(int);

	void setEEGTeeth(int);
	void setEEGTeethChange(int);

	void setEEGEyebrow(int);
	void setEEGEyebrowChange(int);

	void setEyeQuadrant(std::string);

	void setState(std::string);
};

