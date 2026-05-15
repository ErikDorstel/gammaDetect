const char *wlanSSID="xxx";
const char *wlanPASS="xxx";

#include "Counter.h"
#include "WLAN.h"
#include "HTTP.h"

void setup() {
  Serial.begin(115200);
  initCounter();
  initWLAN();
  initHTTP();
  initWebSockets(); }

void loop() { counterWorker(); wlanWorker(); httpWorker(); webSocketsWorker(); }
