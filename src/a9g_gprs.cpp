/*
 * Only GPRS related AT command interface functions
 */

#include "a9gdriver.h"

void A9Gdriver::NET_attach(bool enable) {
  _sendCommEnd(F("AT+CGATT=1"));
  while (!_waitForRx("OK"))
    ;
}

void A9Gdriver::NET_setPDP(int profile, const char *APN) {
  _sendComm(F("AT+CGDCONT="));
  _sendComm(String(profile));
  _sendComm(F(", \"IP\", \""));
  _sendComm(String(APN));
  _sendCommEnd(F("\""));
  while (!_waitForRx("OK"))
    ;
}

void A9Gdriver::NET_activatePDP(int profile, bool activate) {
  _sendComm(F("AT+CGACT="));
  _sendComm(String(1));
  _sendComm(F(","));
  _sendCommEnd(activate ? "1" : "0");
  while (!_waitForRx("OK"))
    ;
}
