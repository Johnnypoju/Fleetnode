/*
 * UART.h
 *
 *  Created on: 30 Oct 2018
 *      Author: jsumusal
 */

#ifndef UART_H_
#define UART_H_
#include "SerialPort.h"
#include <vector>
using namespace std;

class UART {
public:
	UART();
	UART(int sertyp);
	virtual ~UART();
	int UART_init(int baud);
	void UART_send(vector<char> data);
	void UART_receive();
	vector<char> UART_get();


private:
	int baud;
	char temp_char;
	SerialPort *serial, *serial2;
	vector<char> serialdata;
	vector<const char*> tempdata;
	bool SFlag = false;
};
#endif /* UART_H_ */

