#include "SensorComm.h"

SensorComm::SensorComm()
{
	eyeCoordinate[0] = 1;
	eyeCoordinate[1] = 1;
}

SensorComm::~SensorComm(){

}

int SensorComm::getEEGTeeth(){ return eegTeeth; }
int SensorComm::getEEGAttention(){ return eegAttention; }
int SensorComm::getEEGEyebrow(){ return eegEyebrow; }

int SensorComm::getEyeActive(){ return eyeActive; }
std::string SensorComm::getEyeQuadrant() { return eyeQuadrant; }
int* SensorComm::getEyeCoordinate(){ return eyeCoordinate; }
int SensorComm::getEyeAngle(){ return eyeAngle; }


void SensorComm::setEEGAttention(int val){ eegAttention = val; }

void SensorComm::setState(std::string msg){

	std::regex rgx("([a-z])([a-z])([a-z_0-9,]*);", std::regex_constants::icase);
	std::smatch match;
	std::string srcDevice, dataType, data;

	if (std::regex_search(msg, match, rgx)) {
		srcDevice = match[1].str();
		dataType = match[2].str();
		data = match[3].str();

		if (srcDevice == "E") {//From EEG
			if (dataType == "T"){//EEG:Teeth
				int state = std::stoi(data);
				eegTeeth = state;
			}
			else if (dataType == "A") {//EEG:Attention
				//int state = std::stoi(data);
				//eegAttention = state;
				eegAttention = 1;
			}
			else if (dataType == "B") {//EEG:Eyebrow
				int state = std::stoi(data);
				eegEyebrow = state;
			}
		}
		else if (srcDevice == "T") {//From eye tracker
			if (dataType == "Q"){//EYE:quadrant
				eyeActive = 1;
				eyeQuadrant = data;
			}
			else if (dataType == "C"){//EYE:Coordiante
				eyeActive = 1;
				std::regex rgxCoord("([0-9]+),([0-9]+)");
				std::smatch matchData;
				std::regex_search(data,matchData,rgxCoord);
				eyeCoordinate[0] = stoi(matchData[0].str());
				eyeCoordinate[1] = stoi(matchData[1].str());
			}
			else if (dataType == "A"){//EYE:Angle
				eyeActive = 1;
				int angle = std::stoi(data);
				eegAttention = angle;
			}
			else if (dataType == "L"){//EYE:data lost
				eyeActive = 0;
			}
		}
		else {
		}
	}
	else {
		std::cout << "invalid message" << std::endl;
	}

}

