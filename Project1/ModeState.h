#pragma once

#include <iostream>
#include <string>

using namespace std;

class ModeState
{
private:
	int mode = 1;

public:
	ModeState();
	~ModeState();
	
	int getMode();
	void setMode(int);
	void setMode(std::string);
	void toggleMode();
	static int toMode(std::string);

};

