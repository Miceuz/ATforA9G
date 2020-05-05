#include "a9gdriver.h"

void A9Gdriver::sendSms(const char* phone, const char* text) {
	_sendCommEnd("AT+CMGF=1");//switch to text mode
	_waitForRx(F("OK"));
	_sendComm("AT+CMGS=\"");
	_sendComm(phone);
	_sendCommEnd("\"");
	_waitForRx(F(">"));
	_sendComm(text);
	_serial.print('\x1A');//end message with Ctrl+Z
	// _sendComm('\x1A');
}
