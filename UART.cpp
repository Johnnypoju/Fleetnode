/*
 * UART.cpp
 *
 *  Created on: 30 Oct 2018
 *      Author: jsumusal
 */

#include "UART.h"
// constructor for UART1 initialization
UART::UART() {
	// TODO Auto-generated constructor stub
	serial = new SerialPort();
	serial->begin(9600);
}
// override constructor for UART1/2 initialization
UART::UART(int sertyp) {
	serial2 = new SerialPort(sertyp);
	serial2->begin(9600);
	SFlag = true;
}

UART::~UART() {
	// TODO Auto-generated destructor stub
}

// sending data to Raspberry via UART
void UART::UART_send(vector<char> data){

	if (SFlag == true){
		for (int i = 0; i < data.size() ; i++){
			serial2->write((const char*)&data[i], 1);
			Board_UARTPutChar(data[i]);
		};
	}
	else {
		for (int i = 0; i < data.size() ; i++){
			serial->write((const char*)&data[i], 1);
		};
	}
}

// getting data from GPS module
void UART::UART_receive() {
	if (SFlag == true){
		serialdata.clear();
		temp_char = serial2->read();
		while (temp_char != '$'){
			temp_char = serial2->read();
		}
		serialdata.push_back(temp_char);
		do {
			temp_char = serial2->read();
			serialdata.push_back(temp_char);
		} while (temp_char != '\n');
		serial->read();
	}
	else {
		// clear vector for data storage
		serialdata.clear();

		temp_char = serial->read();
		// check if start of EMEA code
		while (temp_char != '$'){
			temp_char = serial->read();
		}
		// if start of commandline start pushing to vector
		serialdata.push_back(temp_char);
		//Board_UARTPutChar(temp_char); // for demo purposes
		do {
			temp_char = serial->read();
			if (temp_char != 255){
				serialdata.push_back(temp_char);
				//Board_UARTPutChar(temp_char); // for demo purposes
			}
		} while (temp_char != '\n');
		serial->read(); // read newline character
	}
}

// getting serialdata vector
vector<char> UART::UART_get(){
	return serialdata;
}
