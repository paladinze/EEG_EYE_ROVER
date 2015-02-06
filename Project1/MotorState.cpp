#include "MotorState.h"


MotorState::MotorState()
{
}

std::string MotorState::forward() {
	return (TYPE + "w" + ENDMSG);
}

std::string MotorState::backward() {
	return (TYPE + "s" + ENDMSG);
}

std::string MotorState::left() {
	return (TYPE + "a" + ENDMSG);
}

std::string MotorState::right() {
	return (TYPE + "d" + ENDMSG);
}