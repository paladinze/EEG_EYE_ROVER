#pragma once
#include <iostream>
#include <string>
#include <Windows.h>
using namespace std;

#define INITIAL_X 95
#define INITIAL_Y 30

//message format: S34,67;
const std::string TYPE = "S";
const std::string DELIMETER = ",";
const std::string ENDMSG = ";";
const int STILL = 0;

class ServoState
{
private:
	//current state
	int posX = INITIAL_X;
	int posY = INITIAL_Y;

public:
	//servo operating range
	static const int maxX = 165;
	static const int minX = 25;
	static const int maxY = 70; //exclusive
	static const int minY = 10; //exclusive
	static const int defaultX = 95;
	static const int defaultY = 30;


	ServoState();
	~ServoState();

	//reset
	std::string reset();

	//Sweep
	std::string sweep();

	//return curret state
	int getX();
	int getY();

	//set state and generate serial command
	std::string setX(int);
	std::string setY(int);
	std::string setXY(int, int);

	//set state(with offset) and generate serial command
	std::string pan(int);
	std::string tilt(int);

	//translate: x,y value => serial message
	static std::string translate(int, int);


};

