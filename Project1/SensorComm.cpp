#include "SensorComm.h"

SensorComm::SensorComm()
{
	eyeCoordinate[0] = 1;
	eyeCoordinate[1] = 1;
}

SensorComm::~SensorComm(){

}

//Get
int SensorComm::getMoveObserveState(){ return moveObserveState; }

int SensorComm::getEEGTeeth(){ return eegTeethCurr; }
int SensorComm::getEEGTeethChange() { return eegTeethChange; }


int SensorComm::getEEGPush(){ return eegPushCurr; }
int SensorComm::getEEGPushChange() { return eegPushChange; }

int SensorComm::getEEGPull(){ return eegPullCurr; }
int SensorComm::getEEGPullChange() { return eegPullChange; }

int SensorComm::getEEGAttention(){ return eegAttentionCurr; }
int SensorComm::getEEGAttentionChange() { return eegAttentionChange; }

int SensorComm::getEEGEyebrow(){ return eegEyebrowCurr; }
int SensorComm::getEEGEyebrowChange() { return eegEyebrowChange; }

int SensorComm::getEyeActive(){ return eyeActive; }
int SensorComm::getEyeActiveChange() { return eyeActiveChange; }

std::string SensorComm::getEyeQuadrant() { return eyeQuadrant; }
int* SensorComm::getEyeCoordinate(){ return eyeCoordinate; }
int SensorComm::getEyeAngle(){ return eyeAngle; }


//Set
void SensorComm::setEEGPull(int val){ eegPullCurr = val; }
void SensorComm::setEEGPullChange(int val){ eegPullChange = val; }

void SensorComm::setEEGPush(int val){ eegPushCurr = val; }
void SensorComm::setEEGPushChange(int val){ eegPushChange = val; }

void SensorComm::setEEGAttention(int val){ eegAttentionCurr = val; }
void SensorComm::setEEGAttentionChange(int val){ eegAttentionChange = val; }

void SensorComm::setEEGTeeth(int val){ eegTeethCurr = val; }
void SensorComm::setEEGTeethChange(int val){ eegTeethChange = val; }

void SensorComm::setEEGEyebrow(int val){ eegEyebrowCurr = val; }
void SensorComm::setEEGEyebrowChange(int val){ eegEyebrowChange = val; }

void SensorComm::setEyeQuadrant(std::string quadrant) { eyeQuadrant = quadrant; }

void SensorComm::setState(std::string msg){

	std::regex rgx("([a-z])([a-z])([a-z_0-9,]*);", std::regex_constants::icase);
	std::smatch match;
	std::string srcDevice, dataType, data;

	if (std::regex_search(msg, match, rgx)) {
		srcDevice = match[1].str();
		dataType = match[2].str();
		data = match[3].str();
		std::cout << "data: " << data << std::endl;

		if (srcDevice == "E") {//From EEG
			if (dataType == "T"){//EEG:Teeth
				int inTeeth = std::stoi(data);
				if (inTeeth == 1 && eegTeethCurr == 0) {
					eegTeethChange = 1;
				}
				else {
					eegTeethChange = 0;
				}
				eegTeethPrev = eegTeethCurr;
				eegTeethCurr = inTeeth;
			}
			else if (dataType == "A") {//EEG:Attention
				int inAttention = std::stoi(data);
				if (inAttention==1 && eegAttentionCurr==0) {
					eegAttentionChange = 1;
				}
				else {
					eegAttentionChange = 0;
				}
				eegAttentionPrev = eegAttentionCurr;
				eegAttentionCurr = inAttention;
			}
			else if (dataType == "B") {//EEG:Eyebrow
				int inEyebrow = std::stoi(data);
				if (inEyebrow == 1 && eegEyebrowCurr == 0) {
					moveObserveState = (moveObserveState + 1) % 2;
					eegEyebrowChange = 1;
				}
				else {
					eegEyebrowChange = 0;
				}
				eegEyebrowPrev = eegEyebrowCurr;
				eegEyebrowCurr = inEyebrow;
			}
			else if (dataType == "P") {
				int inPush = std::stoi(data);
				if (inPush == 1 && eegPushCurr == 0) {
					eegPushChange = 1;
				}
				else {
					eegPushChange = 0;
				}
				eegPushPrev = eegPushCurr;
				eegPushCurr = inPush;
			}
			else if (dataType == "L") {
				int inPull = std::stoi(data);
				if (inPull == 1 && eegPullCurr == 0) {
					eegPullChange = 1;
				}
				else {
					eegPullChange = 0;
				}
				eegPullPrev = eegPullCurr;
				eegPullCurr = inPull;
			}
		}
		else if (srcDevice == "T") {//From eye tracker
			if (dataType == "Q"){//EYE:quadrant
				eyeActive = 1;
				eyeQuadrant = data;
				//std::cout << "Quadrant set to inside: " << eyeQuadrant << std::endl;
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
				eyeAngle = angle;
			}
			else if (dataType == "L"){//EYE:data lost
				eyeActive = 0;
			}
		}
		else {
			eyeActive = 0;
		}
		//std::cout << "Quadrant set to: " << eyeQuadrant << std::endl;
	}
	else {
		std::cout << "invalid message" << std::endl;
	}

}

