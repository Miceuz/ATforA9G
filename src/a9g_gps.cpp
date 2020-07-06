/*
 * Only GPS related AT command interface functions
 */

#include "Arduino.h"
#include "a9gdriver.h"

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
