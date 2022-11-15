#include "menu_stopwatch.h"

static uint16_t current_value = 30;

static void
show_countdown(uint16_t value) {
  display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.setFont(&DSEG7_Classic_Bold_53);
  display.setTextColor(DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  display.setCursor(50, 100);

  unsigned long previous_millis = 0;

  while (value) {
    if (digitalRead(BACK_BTN_PIN) == 1) {
      return;
    }

    unsigned long current_millis = millis();

    if (current_millis - previous_millis > 1000) {
      previous_millis = current_millis;
      display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
      display.setCursor(5, 120);
      display.printf("%02d:%02d", value / 60, value % 60);
      display.display(true);
      value--;
    }
  }
  vibrate_motor(100, 5);
}

void
menu_stopwatch(void) {
  bool timeout = false;

  display.init(0, true); //_initial_refresh to false to prevent full update on init
  display.setFullWindow();
  display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.setFont(&DSEG7_Classic_Bold_53);
  display.setTextColor(DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);

  gui_state = STATE_MENU_APP;

  pinMode(DOWN_BTN_PIN, INPUT);
  pinMode(UP_BTN_PIN, INPUT);
  pinMode(MENU_BTN_PIN, INPUT);
  pinMode(BACK_BTN_PIN, INPUT);

  display.setCursor(5, 120);
  display.printf("%02d:%02d", current_value / 60, current_value % 60);
  display.display(false);

  long last_timeout = millis();

  while (!timeout) {

    if (millis() - last_timeout > MENU_STOPWATCH_TIMEOUT)
      timeout = true;

    if (digitalRead(BACK_BTN_PIN) == 1) {
      break;
    }

    if (digitalRead(MENU_BTN_PIN) == 1) {
      show_countdown(current_value);
      display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
      display.setCursor(5, 120);
      display.printf("%02d:%02d", current_value / 60, current_value % 60);
      display.display(true);
      last_timeout = millis();
    }

    if (digitalRead(DOWN_BTN_PIN) == 1) {
      current_value = current_value <= 30 ? 30 : current_value - 30;
      display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
      display.setCursor(5, 120);
      display.printf("%02d:%02d", current_value / 60, current_value % 60);
      display.display(true);
      last_timeout = millis();
    }

    if (digitalRead(UP_BTN_PIN) == 1) {
      current_value = current_value + 30;
      display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
      display.setCursor(5, 120);
      display.printf("%02d:%02d", current_value / 60, current_value % 60);
      display.display(true);
      last_timeout = millis();
    }
  }

  display.display(false); //full refresh
  display.hibernate();
  show_menu(menu_index, false);
}
