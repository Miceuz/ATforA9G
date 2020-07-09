/*
 * Utilitary and inner-working functions
 */
#include "a9gdriver.h"

void A9Gdriver::_dropRx() {

  // while (_serial.available()) {
  //   uint8_t c = static_cast<uint8_t>(_serial.read());
  //   Serial.print((char)c);
  // }
}

void A9Gdriver::_flushTx() { _serial.flush(); }

void A9Gdriver::_sendComm(String command) {
  _dropRx();
  Serial.print("[send]:");
  Serial.print(command);
  _serial.print(command);
  _flushTx();
}

void A9Gdriver::_sendCommEnd(String command) {
  _sendComm(command);
  _serial.print(F("\r"));
  Serial.println();
}

void A9Gdriver::_sendBuffer(const char *buffer, size_t size) {
  char *out = (char *)buffer;

  for (char *it = out; it < buffer + size; it += 4)
    *out++ = "0123456789abcdef"[(it[0] != '0') * 8 + (it[1] != '0') * 4 +
                                (it[2] != '0') * 2 + (it[3] != '0') * 1];

  _serial.write(out, size / 4);
  _serial.flush();
}

void A9Gdriver::_processCmeError(String line) {
  // todo
}

//+CTZV:20/07/06,11:31:15,+03

//+MQTTDISCONNECTED: 256
//+MQTTPUBLISH:2,device/req/7,509,labaslabas
void A9Gdriver::_processMqttMessage(String line) {
  String mqtt_idx = line.substring(strlen(MQTTPUBLISH), line.indexOf(","));
  int topic_start = line.indexOf(",") + 1;
  int topic_end = line.indexOf(",", line.indexOf(",") + 1);
  String topic = line.substring(topic_start, topic_end);

  int length_end = line.indexOf(",", topic_end + 1);
  String length = line.substring(topic_end, length_end);

  String body = line.substring(length_end + 1);
  body.getBytes(mqtt_message, body.length() + 1);
  mqtt_message[body.length() + 1] = 0;
  // Serial.println(String("mqtt idx:") + mqtt_idx);
  // Serial.println(String("mqtt topic:") + topic);
  // Serial.println(String("mqtt body len:") + length);
  // Serial.println(String("mqtt body:" + body));
}

// +GPSRD:$GNGGA,143628.000,5441.6781,N,02526.4747,E,1,5,7.26,164.4,M,27.4,M,,*47
void A9Gdriver::_processGps(String line) {
  String gngga = line.substring(line.indexOf("$GNGGA"));
  gngga.getBytes(gngga_message, gngga.length() + 1);
  gngga_message[gngga.length() + 1] = 0;
}

void A9Gdriver::_processRxLine(String line) {
  if (line.indexOf(CME_ERROR) >= 0) {
    cme_error = true;
    _processCmeError(line);
  } else if (line.indexOf(MQTTPUBLISH) >= 0) {
    mqtt_message_available = true;
    _processMqttMessage(line);
  } else if (line.indexOf(GPSRD) >= 0) {
    gps_available = true;
    _processGps(line);
  } else if (line.indexOf(MQTT_DISCONNECTED) >= 0) {
    mqtt_disconnected = true;
  }
}

bool A9Gdriver::_waitForRx(String needle, unsigned long timeout) {
  needle.trim();
  String response = _serial.readStringUntil('\n');
  response.trim();
  Serial.print("[got]:<");
  Serial.print(response);
  Serial.println(">");

  // if (response.length() == 0) {
  //   response = _serial.readStringUntil('\n');
  //   Serial.println(response);
  //   response.trim();
  // }

  _processRxLine(response);

  if (response.indexOf(needle) >= 0) {
    return true;
  }
  // Serial.print("[response length:");
  // Serial.print(response.length());
  // Serial.print("]:");
  // Serial.print(response);
  // Serial.print(" result:");
  // Serial.println(response.indexOf(needle) >=0);
  return false;
}

bool A9Gdriver::_waitForOk() {
  uint32_t retries = 0;
  while (!_waitForRx("OK") && retries++ < max_retries)
    ;
  return retries < max_retries;
}

void A9Gdriver::_sendLongString(const char *str) {
  for (uint32_t k = 0; k < strlen_P(str); k++) {
    char tmpChar = pgm_read_byte_near(str + k);
    _serial.print(tmpChar);
  }
}
