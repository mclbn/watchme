#include "wifi.h"

bool
connectWiFi(void) {
  WIFI_CONFIGURED = false;

  for (uint8_t i = 0; know_wifi_hotspots[i].ssid; i++) {
    WiFi.begin(know_wifi_hotspots[i].ssid, know_wifi_hotspots[i].pass);
    if (WiFi.waitForConnectResult() == WL_CONNECTED) {
      WIFI_CONFIGURED = true;
      return WIFI_CONFIGURED;
    }
    WiFi.mode(WIFI_OFF);
  }
  btStop();
  return WIFI_CONFIGURED;
}

void
check_wireless_interfaces(void) {
  connectWiFi();
  WiFi.mode(WIFI_OFF);
  btStop();
}
