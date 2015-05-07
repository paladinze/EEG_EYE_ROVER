#pragma once
#include <iostream>
#include <string>
#include <algorithm>

const std::string TYPE = "S";
const std::string DELIMETER = ",";
const std::string ENDMSG = ";";
const int STILL = 0;

class ServoState
{
private:
	//current state
	int posX = initX;
	int posY = initY;

public:
	//max range for roration
	static const int maxX = 140;
	static const int initX = 100;
	static const int minX = 60;

	static const int maxY = 110;
	static const int initY = 85;
	static const int minY = 75;

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

