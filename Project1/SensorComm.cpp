#include "SensorComm.h"

SensorComm::SensorComm()
{
}

SensorComm::~SensorComm(){

}

void SensorComm::setEEGTeethState(){
	//setEEGTeethState = 
}

void SensorComm::setState(string msg){

	if (msg == "TQa;") {
		eyeQuadrant = "a";
	}
	else if (msg == "TQd;"){
		eyeQuadrant = "d";
	}
	else if (msg == "TQw;"){
		eyeQuadrant = "w";
	}
	else if (msg == "TQs;"){
		eyeQuadrant = "s";
	}
	else if (msg == "TQN;"){
		eyeQuadrant = "N";
	}
	else if (msg == "TQ0;"){
		eyeQuadrant = "0";
	}

}

void SensorComm::translateSensorToState(string msg){
	


}

