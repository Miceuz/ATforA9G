#include "a9gdriver.h"

bool A9Gdriver::isNetworkConnected() {
  _sendCommEnd("AT+CREG?");
  return _waitForRx("+CREG: 1,1");
}

bool A9Gdriver::queryRSSI() {
  _sendCommEnd("AT+CSQ");
  return _waitForRx("+CSQ:", 5000);
}
