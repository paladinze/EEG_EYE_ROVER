#include "ModeState.h"


ModeState::ModeState()
{
}

ModeState::~ModeState(){

}

int ModeState::getMode(){
	return modeCurr;
}

int ModeState::getChange(){
	return modeChange;
}

void ModeState::setChange(int val) {
	modeChange = val;
}

void ModeState::setMode(int val) {
	modePrev = modeCurr;
	modeCurr = val;
	if (modeCurr != modePrev)
	{
		modeChange = 1;
		cout << "INFO: mode change to " << modeCurr << endl;
	}
	else {
		modeChange = 0;
	}

}

void ModeState::toggleMode() {
	modePrev = modeCurr;
	modeCurr = (modeCurr % 3) + 1;
	if (modeCurr != modePrev)
	{
		modeChange = 1;
		cout << "INFO: mode change to " << modeCurr << endl;
	}
	else {
		modeChange = 0;
	}
}

