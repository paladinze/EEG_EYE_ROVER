#pragma once
#include <iostream>
#include <string>

using namespace std;

class MotorState
{
	const std::string TYPE = "M";
	const std::string DELIMETER = ",";
	const std::string ENDMSG = ";";
	const int STILL = 0;

public:
	MotorState();
	~MotorState();
	
	std::string forward();
	std::string backward();
	std::string left();
	std::string right();
	
};

