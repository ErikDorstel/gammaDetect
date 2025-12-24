#include <WiFi.h>

int wifiStatus;

void initWLAN() {
  WiFi.begin(apSSID,apPW); }

void wlanWorker() {
  static uint32_t wlanTimer1=millis()+60000;
  static uint32_t wlanTimer2=millis()+5000;
  if (millis()>=wlanTimer1) { wlanTimer1=millis()+60000;
    if (WiFi.status()!=WL_CONNECTED) { WiFi.disconnect(); initWLAN(); } }
  if (millis()>=wlanTimer2) { wlanTimer2=millis()+5000;
    if (WiFi.status()!=wifiStatus) { wifiStatus=WiFi.status();
      if (wifiStatus==WL_CONNECTED) { Serial.print("IP: "); Serial.println(WiFi.localIP()); Serial.println("WiFi Connected."); }
      else { Serial.println("WiFi Disconnected."); } } } }
