#include "menu_battery.h"

void
menu_battery_status() {
  display.init(0, false); //_initial_refresh to false to prevent full update on init
  display.setFullWindow();
  display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  display.setCursor(20, 30);
  display.println("Battery Voltage:");
  float voltage = get_battery_voltage();
  display.setCursor(70, 80);
  display.print(voltage);
  display.println("V");
  display.display(false); //full refresh
  display.hibernate();

  gui_state = STATE_MENU_APP;
}
