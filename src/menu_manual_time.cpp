#include "menu_manual_time.h"

void
menu_set_time(void) {
  gui_state = STATE_MENU_APP;

  RTC.read(currentTime);

  int8_t minute = currentTime.Minute;
  int8_t hour = currentTime.Hour;
  int8_t day = currentTime.Day;
  int8_t month = currentTime.Month;
  int8_t year = currentTime.Year;

  int8_t setIndex = SET_HOUR;

  int8_t blink = 0;

  pinMode(DOWN_BTN_PIN, INPUT);
  pinMode(UP_BTN_PIN, INPUT);
  pinMode(MENU_BTN_PIN, INPUT);
  pinMode(BACK_BTN_PIN, INPUT);

  display.init(0, true); //_initial_refresh to false to prevent full update on init
  display.setFullWindow();

  while(1) {

    if (digitalRead(MENU_BTN_PIN) == 1) {
      setIndex++;
      if (setIndex > SET_DAY) {
        break;
      }
    }
    if (digitalRead(BACK_BTN_PIN) == 1) {
      if (setIndex != SET_HOUR) {
        setIndex--;
      }
    }

    blink = 1 - blink;

    if (digitalRead(DOWN_BTN_PIN) == 1) {
      blink = 1;
      switch (setIndex) {
      case SET_HOUR:
	hour == 23 ? (hour = 0) : hour++;
	break;
      case SET_MINUTE:
	minute == 59 ? (minute = 0) : minute++;
	break;
      case SET_YEAR:
	year == 99 ? (year = 0) : year++;
	break;
      case SET_MONTH:
	month == 12 ? (month = 1) : month++;
	break;
      case SET_DAY:
	day == 31 ? (day = 1) : day++;
	break;
      default:
	break;
      }
    }

    if (digitalRead(UP_BTN_PIN) == 1) {
      blink = 1;
      switch (setIndex) {
      case SET_HOUR:
	hour == 0 ? (hour = 23) : hour--;
	break;
      case SET_MINUTE:
	minute == 0 ? (minute = 59) : minute--;
	break;
      case SET_YEAR:
	year == 0 ? (year = 99) : year--;
	break;
      case SET_MONTH:
	month == 1 ? (month = 12) : month--;
	break;
      case SET_DAY:
	day == 1 ? (day = 31) : day--;
	break;
      default:
	break;
      }
    }

    display.fillScreen(GxEPD_BLACK);
    display.setTextColor(GxEPD_WHITE);
    display.setFont(&DSEG7_Classic_Bold_53);

    display.setCursor(5, 80);
    if (setIndex == SET_HOUR) {//blink hour digits
      display.setTextColor(blink ? GxEPD_WHITE : GxEPD_BLACK);
    }
    if (hour < 10) {
      display.print("0");
    }
    display.print(hour);

    display.setTextColor(GxEPD_WHITE);
    display.print(":");

    display.setCursor(108, 80);
    if (setIndex == SET_MINUTE) {//blink minute digits
      display.setTextColor(blink ? GxEPD_WHITE : GxEPD_BLACK);
    }
    if (minute < 10) {
      display.print("0");
    }
    display.print(minute);

    display.setTextColor(GxEPD_WHITE);

    display.setFont(&FreeMonoBold9pt7b);
    display.setCursor(45, 150);
    if (setIndex == SET_YEAR) {//blink minute digits
      display.setTextColor(blink ? GxEPD_WHITE : GxEPD_BLACK);
    }
    display.print(2000+year);

    display.setTextColor(GxEPD_WHITE);
    display.print("/");

    if (setIndex == SET_MONTH) {//blink minute digits
      display.setTextColor(blink ? GxEPD_WHITE : GxEPD_BLACK);
    }
    if (month < 10) {
      display.print("0");
    }
    display.print(month);

    display.setTextColor(GxEPD_WHITE);
    display.print("/");

    if (setIndex == SET_DAY) {//blink minute digits
      display.setTextColor(blink ? GxEPD_WHITE : GxEPD_BLACK);
    }
    if (day < 10) {
      display.print("0");
    }
    display.print(day);
    display.display(true); //partial refresh
  }

  display.hibernate();

  tmElements_t tm;
  tm.Month = month;
  tm.Day = day;
  tm.Year = year;
  tm.Hour = hour;
  tm.Minute = minute;
  tm.Second = 0;

  RTC.set(tm);

  show_menu(menu_index, false);
}
