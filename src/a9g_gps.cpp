/*
 * Only GPS related AT command interface functions
 */

#include "Arduino.h"
#include "a9gdriver.h"

#define GPS_READ_INTERVAL_S 1
#define GPS_READ_INTERVAL_MS GPS_READ_INTERVAL_S * 1000 

bool A9Gdriver::_isGpsTimeout() {
	return millis() - gpsQueryStartMs > GPS_READ_INTERVAL_MS;
}

bool A9Gdriver::GPS_getLocation(char *rx_buf) {
	switch (gpsQueryState) {
	    case STATE_IDLE:
			GPS_setEnabled(true);
			GPS_setReadInterval(GPS_READ_INTERVAL_S);
			gpsQueryStartMs = millis();
			gpsQueryState = STATE_WAIT_RESPONSE;
			break;
	    case STATE_WAIT_RESPONSE:
			if(_getGPSData(rx_buf) || _isGpsTimeout()) {
				GPS_setReadInterval(0);
				gpsQueryState = STATE_IDLE;
				return !_isGpsTimeout();
			}
	      break;
	}

	return false;
}

// +GPSRD:$GNGGA,143628.000,5441.6781,N,02526.4747,E,1,5,7.26,164.4,M,27.4,M,,*47
bool A9Gdriver::_getGPSData(char *output) {
	String response = _serial.readStringUntil('\n');
	response.trim();

	if(response.indexOf("+GPSRD") == 0) {
		String s = response.substring(response.indexOf("$GNGGA"));
		s.getBytes((unsigned char*)output, s.length()+1);
		output[s.length()+1] = 0;

		delayMicroseconds(100);
		while(_serial.available()) {
			_serial.read();
			delayMicroseconds(100);
		}
		return true;
	} else {
		return false;
	}
}

bool A9Gdriver::GPS_setEnabled(bool enable) {
	_sendComm(F("AT+GPS="));
	_sendCommEnd(enable ? "1" : "0");
	return _waitForRx(F("OK"));
}

bool A9Gdriver::GPS_setReadInterval(uint32_t seconds) {
	_sendComm("AT+GPSRD=");
	_sendCommEnd(String(seconds));
	return _waitForRx("OK");
}
