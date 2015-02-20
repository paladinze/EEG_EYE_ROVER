
#ifndef BTSERIAL_H
#define BTSERIAL_H

#include <string>
#include <iostream>
#include <Windows.h> //windows API header
using namespace std;

class BtSerial
{
private:
	//connection handle
	HANDLE serialPort;
	int connected = 0; 

	//connection setting
	LPCSTR comport;
	int baudrate;
	int bytesize;
	int stopbits;
	int parity;

public:
	BtSerial(); //set defualt params
	BtSerial(LPCSTR);
	~BtSerial(); //close connection
	
	int open_port(); //create serial handle
	int setup_params(); //set serial handle
	int timeout(); //set timeout setting
	DWORD write_port(char); // write char to port
	DWORD write_port(std::string); //write cmd to port
	DWORD write_port(std::string, int); // write the same cmd multiple times
	DWORD read_port(); // read from port
	void close_port(); // close handle
	int isConnected();
	void setState(int);

};


#endif