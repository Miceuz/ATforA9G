/*
 * Ai-Thinker A9(G) Library using AT Commands
 *
 * Based on:
 *
 * Series Module AT Instruction Set V1.1 (cn. spec)
 * https://wiki.ai-thinker.com/_media/gprs/at%E6%8C%87%E4%BB%A4%E9%9B%8620180825.pdf
 *
 * GPRS Series Module AT Instruction Set V1.0 (en. spec)
 * https://wiki.ai-thinker.com/_media/gprs/b000at00a0_gprs_series_module_at_instruction_set_v1.0.pdf
 *
 * A6/A9/A9G GSM/GPRS operation example (cn. wiki)
 * https://wiki.ai-thinker.com/gprs/examples
 *
 */

#ifndef A9G_AT_COMMANDS_H
#define A9G_AT_COMMANDS_H

#include "ArduinoJson.h"
#include "a9g-codes.h" // Check this header for error/status codes
#include <Arduino.h>
#include <stdbool.h>

constexpr const char *const GPSRD = "+GPSRD:";
constexpr const char *const MQTTPUBLISH = "+MQTTPUBLISH:";
constexpr const char *const CME_ERROR = "+CME ERROR:";
constexpr const char *const MQTT_DISCONNECTED = "+MQTTDISCONNECTED:";

class A9Gdriver {
public:
  //--------------------------
  // GENERAL LIBRARY FUNCTIONS

  /**
   * Register the software/hardware Serial connected to the A9(G),
   * to be driven directly by the library. The Serial MUST had been
   * initialized externally at 115200 baud, prior to calling init()
   */
  A9Gdriver(Uart &serial);

  bool init();

  void ATmode(bool enabled); // "+++" and "ATO"

  bool isNetworkConnected();
  bool queryRSSI();
  // Starting from here, the public functions are ordered by the category
  // given in the chinese AT Instruction Set and then the english version

  //--------------------------------------------------
  // 2. GENERAL PURPOSE INSTRUCTIONS (a9g_general.cpp)

  bool A_attention(); // AT

  // void A_write(); // AT&W

  // void A_factoryReset();  // AT&F

  // void A_defaults();  //ATZ

  // void A_getManufacturerInfo();   // AT+CGMI

  // void A_getProductModel();   // AT+CGMM

  // void A_getSoftwareVersionInfo();    // AT+CGMR

  // void A_getIMEI();   // AT+CGSN. AT+EGMR Will NOT be implemented

  void A_setEchoMode(bool enabled); // ATE

  //------------------------------------------------
  // 3. STATE CONTROL INSTRUCTIONS (a9g_general.cpp)

  void sendRst(A9G_shdn_level_t type); // AT+RST

  //------------------------------------------------------------
  // 4. SIM-CARD INSTRUCTIONS and (en. spec) 3. SIM/PBK Commands

  //---------------------------------------------------------------
  // 5. CALL CONTROL INSTRUCTIONS and (en. spec) 6. STK/SS Commands

  //--------------------
  // 6. SMS INSTRUCTIONS
  void sendSms(const char *phone, const char *text);

  //---------------------------
  // 7. PHONE BOOK INSTRUCTIONS

  //-------------------------------------------------------------------------------
  // 8. GPRS INSTRUCTIONS and (en. spec) 5. Network Service Commands
  // (a9g_gprs.cpp)

  void NET_attach(bool enable); // CGATT

  void NET_setPDP(int profile, const char *APN); // CGDCONT

  void NET_activatePDP(int profile, bool activate); // CGACT

  //----------------------------------
  // 9. GPS INSTRUCTIONS (a9g_gps.cpp)

  bool GPS_setEnabled(bool enable); // GPS
  bool GPS_setReadInterval(uint32_t seconds);
  //------------------------
  // 10. TCP/IP INSTRUCTIONS

  //----------------------------------------------------
  // FTP INSTRUCTIONS (from en. spec) 9. TCP/IP Commands

  //-----------------------------------------------------
  // 11. (wit cloud?) INSTRUCTIONS (not available for A6)

  //----------------------
  // 12. MQTT INSTRUCTIONS

  bool MQTT_connect(const char *server, uint16_t port, String clientID,
                    uint16_t aliveSeconds, bool cleanSession,
                    const char *username, const char *password); // MQTTCONN
  bool MQTT_connect(const char *server, uint16_t port, String clientID,
                    uint16_t aliveSeconds, bool cleanSession); // MQTTCONN

  bool MQTT_pub(const char *topic, String payload, uint8_t qos, bool dup,
                bool remain); // MQTTPUB
  bool MQTT_pub(const char *topic, StaticJsonDocument<1024> &payload,
                uint8_t qos, bool dup, bool remain);

  bool MQTT_sub(String topic, bool sub, uint8_t qos); // MQTTSUB

  void MQTT_disconnect(); // MQTTDISCONN

  //------------------
  // HTTP INSTRUCTIONS

  void HTTP_sendGet(String url); // HTTPGET

  void HTTP_sendPost(String url, String contentType,
                     String bodyContent); // HTTPPOST

  void clearMqttMessage() { mqtt_message_available = false; };
  void clearCmeError() { cme_error = false; };
  void clearGps() { gps_available = false; };
  void clearMqttDsiconnected() { mqtt_disconnected = false; };

  bool isMqttMessage() { return mqtt_message_available; };
  bool isCmeError() { return cme_error; };
  bool isGps() { return gps_available; };
  bool isMqttDisconnected() { return mqtt_disconnected; };

  uint8_t *getMqttMessage() {
    // uint8_t *c =
    //     (uint8_t
    //          *)"{\"device_id\": 7, \"request_id\": 10, \"current_command\": "
    //            "{\"request\": \"multiple_pids\", \"payload\": [\"ATSH "
    //            "7b0\", \"STP "
    //            "33\", \"21 03\"]}, \"time_issued\": 741246660}";
    // memcpy(mqtt_message, c, strlen((char *)c));
    return mqtt_message;
  };
  uint8_t *getGngga() { return gngga_message; };

protected:
  Uart &_serial;
  enum State_t {
    STATE_IDLE,
    STATE_WAIT_RESPONSE,
  };

  bool mqtt_message_available = false;
  bool cme_error = false;
  bool gps_available = false;
  bool mqtt_disconnected = false;

  uint8_t mqtt_message[512];
  uint8_t gngga_message[512];

  uint32_t max_retries = 20;

  bool _echo;
  void _processCmeError(String line);
  void _processMqttMessage(String line);
  void _processGps(String line);
  void _processRxLine(String line);
  void _dropRx();
  void _flushTx();
  void _sendComm(String command);
  void _sendCommEnd(String command);
  void _sendLongString(const char *str);
  void _sendBuffer(const char *buffer, size_t size);
  bool _waitForRx(String needle, unsigned long timeout = 1000);
  bool _waitForOk();
  bool _getGPSData(char *s);
  bool _isGpsTimeout();
};

#endif // A9G_AT_COMMANDS_H !def
