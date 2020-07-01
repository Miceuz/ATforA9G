/*
 * Only general and mixed AT command interface functions
 * (not inner-working functions!!)
 */

#include "a9gdriver.h"

A9Gdriver::A9Gdriver(Uart &serial) : _serial(serial) {}

bool A9Gdriver::init() {
  // A9G restart is assumed before init
  // A9G takes some time to startup. Bang Set echo off command at it until it
  // responds.
  uint32_t retries = 0;
  _serial.begin(115200);
  while (!_serial)
    ;
  _serial.setTimeout(1000);

  while (!_waitForRx("READY")) {
    delay(500);
  }

  A_setEchoMode(false);
  while (!_waitForRx("OK") && retries++ < 20) {
    A_setEchoMode(false);
    delay(500);
  }
  return retries < 20;
}

void A9Gdriver::A_attention() {
  _sendCommEnd(F("AT"));
  while (!_waitForRx("OK"))
    ;
}

void A9Gdriver::A_setEchoMode(bool enabled) {
  _sendComm(F("ATE"));
  _sendCommEnd(String((uint8_t)enabled));
  _echo = enabled;
}

void A9Gdriver::sendRst(A9G_shdn_level_t type) {
  switch (type) {
  case RESTART:
    _sendCommEnd(F("AT+RST=1"));
    break;

  case SHUTDOWN:
    _sendCommEnd(F("AT+RST=2"));
    break;

  default:
    break;
  }
}

void A9Gdriver::ATmode(bool enabled) {
  delay(3050);
  _sendCommEnd(enabled ? F("+++") : F("ATO"));
  delay(3050);
}
