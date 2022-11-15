#include "ntp_time.h"

void
update_rtc_from_ntp(time_t ntp_time) {
  struct tm * timeinfo;

    timeinfo = localtime(&ntp_time);

    tmElements_t tm;
    tm.Month = timeinfo->tm_mon + 1;
    tm.Day = timeinfo->tm_mday;
    tm.Year = timeinfo->tm_year - 100;
    tm.Hour = timeinfo->tm_hour;
    tm.Minute = timeinfo->tm_min;
    tm.Second = timeinfo->tm_sec;

    RTC.set(tm);
}

time_t
get_ntp_time(void) {
  time_t ntp_time = 0;

  if (connectWiFi()) {
    WiFiUDP ntp_udp;
    NTPClient time_client(ntp_udp, NTP_SERVER, TIME_OFFSET);

    // We configure time savings (France)
    TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120}; // Summer
    TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60}; // Standard
    Timezone CE(CEST, CET);

    time_client.begin();
    time_client.forceUpdate();
    //ntp_time = time_client.getEpochTime();
    ntp_time = CE.toLocal(time_client.getEpochTime());

    time_client.end();

    //turn off radios
    WiFi.mode(WIFI_OFF);
    // Are we sure about that ? Isn't btStop fow BT ?
    btStop();
  }
  return ntp_time;
}









