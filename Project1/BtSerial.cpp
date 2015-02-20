#include "BtSerial.h"

BtSerial::BtSerial()
{
	comport = "COM7";
	baudrate = CBR_9600;
	bytesize = 8;
	stopbits = ONESTOPBIT;
	parity = NOPARITY;
}

BtSerial::BtSerial(LPCSTR portname)
{
	comport = portname;
	baudrate = CBR_9600;
	bytesize = 8;
	stopbits = ONESTOPBIT;
	parity = NOPARITY;
}

BtSerial::~BtSerial()
{
	close_port();
}

int BtSerial::open_port(){

	serialPort = CreateFile(comport,	//port name
		GENERIC_READ | GENERIC_WRITE,	//IO mode
		0,	//always 0
		0,	//always 0
		OPEN_EXISTING,	//use existing port
		FILE_ATTRIBUTE_NORMAL,	//do nothing fancy
		0);

	if (serialPort == INVALID_HANDLE_VALUE){
		if (GetLastError() == ERROR_FILE_NOT_FOUND){
			printf("\tError: port does not exist! \n");
		}
		else {//handle other errors
			printf("\tError: serial connection failed \n");
		}	
		return 0;
	}
	else {
		printf("\tINFO: serial connection successful! \n");
		return 1;
	}
}

int BtSerial::setup_params() {

	DCB dcbSerialParams = { 0 };	//create DCB struct
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);	//set DCB size	
	if (!GetCommState(serialPort, &dcbSerialParams)) {			//get current setting 
		printf("\tError: cannot get current serial port state! \n");
		return 0;
	}
	dcbSerialParams.BaudRate = baudrate;
	dcbSerialParams.ByteSize = bytesize;
	dcbSerialParams.StopBits = stopbits;
	dcbSerialParams.Parity = parity;

	if (!SetCommState(serialPort, &dcbSerialParams)){
		printf("\tError: cannot set serial port state! \n");
		return 0;
	}
	else {
		printf("\tINFO: set params successful \n");
		setState(1);
		return 1;
	}
}

int BtSerial::timeout() {

	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if (!SetCommTimeouts(serialPort, &timeouts)){
		printf("\tError: cannot set serial port timeout! \n");
		return 0;
	}
	else {
		printf("\tINFO: set timeout setting successful \n");
		return 1;
	}
}

DWORD BtSerial::write_port(char input) {
	DWORD bytesWriten = 0;
	const int size_buff = 1;
	char buffer[size_buff];
	for (int i = 0; i < size_buff; i++) {
		buffer[i] = input;
	}

	if (!WriteFile(serialPort, buffer, size_buff, &bytesWriten, NULL)){
		printf("\tError: cannot write to serial port! \n");
	}
	return bytesWriten;
}

DWORD BtSerial::write_port(std::string inStr) {
	DWORD bytesWriten = 0;
	const int size_buff = 1;
	char buffer[size_buff];
	
	//cout << inStr << endl;

	for (char& c : inStr) {
		buffer[0] = c;
		//cout << c << endl;
		if (!WriteFile(serialPort, buffer, size_buff, &bytesWriten, NULL)){
			printf("\tError: cannot write to serial port! \n");
		}
	}
	return bytesWriten;
}


DWORD BtSerial::write_port(std::string inStr, int times) {
	DWORD bytesWriten = 0;
	const int size_buff = 1;
	char buffer[size_buff];
	std::string concatStr = "";

	for (int i = 0; i < times; i++) {
		concatStr = concatStr + inStr;
	}

	//cout << inStr << endl;

	for (char& c : concatStr) {
		buffer[0] = c;
		//cout << c << endl;
		if (!WriteFile(serialPort, buffer, size_buff, &bytesWriten, NULL)){
			printf("\tError: cannot write to serial port! \n");
		}
	}
	return bytesWriten;
}



DWORD BtSerial::read_port(){
	DWORD bytesReaded = 0;
	return bytesReaded;
}

void BtSerial::close_port() {
	if (serialPort != INVALID_HANDLE_VALUE) {
		printf("\tINFO: serial connection closed! \n");
		CloseHandle(serialPort);
		setState(0);
	}
}

int BtSerial::isConnected() {
	return connected;
}

void BtSerial::setState(int val) {
	connected = val;
}

