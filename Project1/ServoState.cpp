#include "ServoState.h"


ServoState::ServoState()
{
}

ServoState::~ServoState()
{
}

//reset
std::string ServoState::reset(){
	return ServoState::translate(INITIAL_X, INITIAL_Y);
}

std::string ServoState::sweep(){
	return (TYPE + "g" + ENDMSG);
}

//return curret state
int ServoState::getX() { return posX; }
int ServoState::getY() { return posY; }

//set state and generate serial command
std::string ServoState::setX(int val) { 
	return ServoState::translate(val, STILL);
}
std::string ServoState::setY(int val) {
	return ServoState::translate(STILL, val);
}
std::string ServoState::setXY(int val_x, int val_y) {
	return ServoState::translate(val_x, val_y);
}

//set state(with offset) and generate serial command
std::string ServoState::pan(int val){
	posX += val;
	return ServoState::translate(posX, STILL);
}

std::string ServoState::tilt(int val){
	posY += val;
	return ServoState::translate(STILL, posY);
}

//translate: x,y value => serial message
std::string ServoState::translate(int val_x, int val_y) {
	return (TYPE + std::to_string(val_x) + DELIMETER + std::to_string(val_y)+ENDMSG);
}



