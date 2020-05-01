/*
 * Utilitary and inner-working functions
 */
#include "a9gdriver.h"


void A9Gdriver::_dropRx()
{
  while (_serial.available())
    _serial.read();
}

void A9Gdriver::_flushTx()
{
  _serial.flush();
}

void A9Gdriver::_sendComm(String command)
{
  _dropRx();
  Serial.print("[send]:");
  Serial.print(command);
  _serial.print(command);
  _flushTx();
}

void A9Gdriver::_sendCommEnd(String command)
{
  _sendComm(command);
  _serial.print(F("\r"));
  Serial.println();
}


void A9Gdriver::_sendBuffer(const char *buffer, size_t size)
{
  char *out = (char *)buffer;

  for (char *it = out; it < buffer + size; it += 4)
    *out++ = "0123456789abcdef"[(it[0] != '0') * 8 +
                                (it[1] != '0') * 4 +
                                (it[2] != '0') * 2 +
                                (it[3] != '0') * 1];

  _serial.write(out, size / 4);
  _serial.flush();
}

bool A9Gdriver::_waitForRx(String needle,unsigned long timeout)
{
  needle.trim();
  String response = _serial.readStringUntil('\n');
  response.trim();
 
  if(response.length() == 0) {
    response = _serial.readStringUntil('\n');
    response.trim();
  }
  Serial.print("[response length:");
  Serial.print(response.length());
  Serial.print("]:");
  Serial.print(response);
  Serial.print(" result:");
  Serial.println(response.indexOf(needle) >=0);
  return response.indexOf(needle) >= 0;
}

void A9Gdriver::_sendLongString(const char *str)
{
  for (uint32_t k = 0; k < strlen_P(str); k++)
  {
    char tmpChar = pgm_read_byte_near(str + k);
    _serial.print(tmpChar);
  }
}
