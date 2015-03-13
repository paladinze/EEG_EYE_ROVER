#pragma once

#include <iostream>
#include <string>

using namespace std;

class ModeState
{
private:
	int modeCurr = 1;
	int modePrev = 1;
	int modeChange = 0;

public:
	ModeState();
	~ModeState();

	int getMode();
	int getChange();
	void setChange(int);
	void setMode(int);
	void setMode(std::string);
	void toggleMode();


};

