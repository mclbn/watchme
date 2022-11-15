#include "menu_ntp_time.h"

void
menu_ntp_time(void) {
  display.init(0, false); //_initial_refresh to false to prevent full update on init
  display.setFullWindow();
  display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  display.setCursor(50, 80);

  time_t ntp_time = get_ntp_time();

  if (ntp_time != 0) {
    update_rtc_from_ntp(ntp_time);
    display.println("Success!");
  } else
    display.println("Failure!");

  delay(1000);

  display.display(false); //full refresh
  display.hibernate();
  show_menu(menu_index, false);
}
