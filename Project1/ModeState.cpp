#include "ModeState.h"


ModeState::ModeState()
{
}

ModeState::~ModeState(){

}

int ModeState::getMode(){
	return mode;
}

void ModeState::setMode(int val) {
	mode = val;
}

void ModeState::setMode(std::string inStr){
	mode = ModeState::toMode(inStr);
}

int ModeState::toMode(std::string inStr) {
	return 0;
}

void ModeState::toggleMode() {
	mode = (mode % 3) + 1;
	cout << "INFO: mode change to " << mode << endl;
}

