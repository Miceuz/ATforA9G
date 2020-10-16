/*
 * Only MQTT related AT command interface functions
 */

#include "a9gdriver.h"

bool A9Gdriver::MQTT_connect(const char *server, uint16_t port, String clientID,
                             uint16_t aliveSeconds, bool cleanSession,
                             const char *username, const char *password) {
  _sendComm(F("AT+MQTTCONN=\""));
  _sendComm(String(server));
  _sendComm(F("\","));
  _sendComm(String(port));
  _sendComm(F(",\""));
  _sendComm(clientID);
  _sendComm(F("\","));
  _sendComm(String(aliveSeconds));
  _sendComm(F(","));
  _sendComm(String((uint8_t)cleanSession));
  _sendComm(F(",\""));
  _sendComm(String(username));
  _sendComm(F("\",\""));
  _sendComm(String(password));
  _sendCommEnd(F("\""));
  return _waitForOk();
}

bool A9Gdriver::MQTT_connect(const char *server, uint16_t port, String clientID,
                             uint16_t aliveSeconds, bool cleanSession) {
  _sendComm(F("AT+MQTTCONN=\""));
  _sendComm(String(server));
  _sendComm(F("\","));
  _sendComm(String(port));
  _sendComm(F(",\""));
  _sendComm(clientID);
  _sendComm(F("\","));
  _sendComm(String(aliveSeconds));
  _sendComm(F(","));
  _sendCommEnd(String((uint8_t)cleanSession));
  return _waitForOk();
}

bool A9Gdriver::MQTT_pub(const char *topic, String payload, uint8_t qos,
                         bool dup, bool remain) {
  _sendComm(F("AT+MQTTPUB=\""));
  _sendComm(String(topic));
  _sendComm(F("\",\""));
  _sendComm(payload);
  _sendComm(F("\","));
  _sendComm(String(qos));
  _sendComm(F(","));
  _sendComm(String((uint8_t)dup));
  _sendComm(F(","));
  _sendCommEnd(String((uint8_t)remain));
  return _waitForOk();
}

class SingleQuoteWriter {
public:
  SingleQuoteWriter(Uart &serial) : _serial(serial){};
  size_t write(uint8_t c) {
    if ('\"' == c) {
      _serial.write('\'');
    } else {
      _serial.write(static_cast<char>(c));
    }
    return 1;
  };

  size_t write(const uint8_t *s, size_t n) {
    _serial.write(reinterpret_cast<const char *>(s), n);
    return n;
  };

private:
  Uart &_serial;
};

bool A9Gdriver::MQTT_pub(const char *topic, StaticJsonDocument<1024> &payload,
                         uint8_t qos, bool dup, bool remain) {
  serializeJson(payload, Serial);

  _sendComm(F("AT+MQTTPUB=\""));
  _sendComm(String(topic));
  _sendComm(F("\",\""));

  SingleQuoteWriter writer(_serial);
  serializeJson(payload, writer);

  _sendComm(F("\","));
  _sendComm(String(qos));
  _sendComm(F(","));
  _sendComm(String((uint8_t)dup));
  _sendComm(F(","));
  _sendCommEnd(String((uint8_t)remain));
  return _waitForOk();
}

bool A9Gdriver::MQTT_sub(String topic, bool sub, uint8_t qos) {
  _sendComm(F("AT+MQTTSUB=\""));
  _sendComm(topic);
  _sendComm(F("\","));
  _sendComm(String((uint8_t)sub));
  _sendComm(F(","));
  _sendCommEnd(String(qos));
  return _waitForOk();
}

void A9Gdriver::MQTT_disconnect() { _sendCommEnd(F("AT+MQTTDISCONN")); }
