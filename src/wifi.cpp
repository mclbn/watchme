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

/*
 * Written by Ahmad Shamshiri
  * with lots of research, this sources was used:
 * https://support.randomsolutions.nl/827069-Best-dBm-Values-for-Wifi
 * This is approximate percentage calculation of RSSI
 * WiFi Signal Strength Calculation
 * Written Aug 08, 2019 at 21:45 in Ajax, Ontario, Canada
 */

static int
dBmtoPercentage(int dBm) {
  const int RSSI_MAX =-50;// define maximum strength of signal in dBm
  const int RSSI_MIN =-100;// define minimum strength of signal in dBm

  int quality;
    if(dBm <= RSSI_MIN)
    {
        quality = 0;
    }
    else if(dBm >= RSSI_MAX)
    {
        quality = 100;
    }
    else
    {
        quality = 2 * (dBm + 100);
   }

     return quality;
}

static bool
is_ap_known(const char * ap_str) {
  for (uint8_t i = 0; know_wifi_hotspots[i].ssid; i++) {
      if (strcmp(ap_str, know_wifi_hotspots[i].ssid) == 0)
        return true;
  }
  return false;
}

bool
check_for_known_ap(void) {

  Serial.println("Starting WiFi scan");
  int16_t nb_ap = WiFi.scanNetworks();

  for (int i = 0; i < nb_ap; ++i) {
    Serial.print(WiFi.SSID(i));
    Serial.print(WiFi.RSSI(i));
    Serial.print(" dBm (");
    Serial.print(dBmtoPercentage(WiFi.RSSI(i)));
    Serial.print("% )");
    if (is_ap_known(WiFi.SSID(i).c_str()))
      Serial.println("[known]");
    else
      Serial.println("[unknown]");  
}

  WiFi.scanDelete();
  Serial.println("WiFi scan done");
}
