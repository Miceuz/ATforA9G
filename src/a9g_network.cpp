#include "a9gdriver.h"

bool A9Gdriver::isNetworkConnected() {
  _sendCommEnd("AT+CREG?");
  return _waitForRx("+CREG: 1,1");
}
