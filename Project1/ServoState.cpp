#include "ServoState.h"

ServoState::ServoState()
{
}

ServoState::~ServoState()
{
}

//reset
std::string ServoState::reset(){
	return ServoState::translate(initX, initY);
}

std::string ServoState::sweep(){
	return (TYPE + "g" + ENDMSG);
}

/*
int clipX(int n){
int lower = ServoState::minX;
int upper = ServoState::maxX;
return 1;
}

int clipY(int n){
int lower = ServoState::minY;
int upper = ServoState::maxY;
return 0;
}
*/

//return curret state
int ServoState::getX() { return posX; }
int ServoState::getY() { return posY; }

//set state and generate serial command
std::string ServoState::setX(int val) {
	val = std::max(ServoState::minX, std::min(val, ServoState::maxX));
	posX = val;
	return ServoState::translate(val, STILL);
}
std::string ServoState::setY(int val) {
	val = std::max(ServoState::minY, std::min(val, ServoState::maxY));
	posY = val;
	return ServoState::translate(STILL, val);
}
std::string ServoState::setXY(int valX, int valY) {
	valX = std::max(ServoState::minX, std::min(valX, ServoState::maxX));
	valY = std::max(ServoState::minY, std::min(valY, ServoState::maxY));
	posX = valX;
	posY = valY;
	return ServoState::translate(valX, valY);
}

//set state(with offset) and generate serial command
std::string ServoState::pan(int val){
	posX += val;
	posX = std::max(ServoState::minX, std::min(posX, ServoState::maxX));
	return ServoState::translate(posX, STILL);
}

std::string ServoState::tilt(int val){
	posY += val;
	posY = std::max(ServoState::minY, std::min(posY, ServoState::maxY));
	return ServoState::translate(STILL, posY);
}

//translate: x,y value => serial message
std::string ServoState::translate(int valX, int valY) {
	return (TYPE + std::to_string(valX) + DELIMETER + std::to_string(valY) + ENDMSG);
}



