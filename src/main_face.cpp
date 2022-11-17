#include "main_face.h"

const uint8_t BATTERY_SEGMENT_WIDTH = 7;
const uint8_t BATTERY_SEGMENT_HEIGHT = 11;
const uint8_t BATTERY_SEGMENT_SPACING = 9;
const uint8_t WEATHER_ICON_WIDTH = 48;
const uint8_t WEATHER_ICON_HEIGHT = 32;

static void
draw_time() {
  int displayHour;

  display.setFont(&DSEG7_Classic_Bold_53);
  display.setCursor(5, 53 + 5);

  if (HOUR_12_24 == 12) {
    displayHour = ((currentTime.Hour + 11) % 12) + 1;
  } else {
    displayHour = currentTime.Hour;
  }

  if (displayHour < 10) {
    display.print("0");
  }

  display.print(displayHour);
  display.print(":");

  if (currentTime.Minute < 10) {
    display.print("0");
  }

  display.println(currentTime.Minute);
}

static void
draw_date() {
  int16_t x1, y1;
  uint16_t w, h;

  display.setFont(&Seven_Segment10pt7b);

  String dayOfWeek = dayStr(currentTime.Wday);
  display.getTextBounds(dayOfWeek, 5, 85, &x1, &y1, &w, &h);
  if (currentTime.Wday == 4) {
    w = w - 5;
  }
  display.setCursor(85 - w, 85);
  display.println(dayOfWeek);

  String month = monthShortStr(currentTime.Month);
  display.getTextBounds(month, 60, 110, &x1, &y1, &w, &h);
  display.setCursor(85 - w, 110);
  display.println(month);

  display.setFont(&DSEG7_Classic_Bold_25);
  display.setCursor(5, 120);
  if (currentTime.Day < 10) {
    display.print("0");
  }
  display.println(currentTime.Day);
  display.setCursor(5, 150);
  display.println(tmYearToCalendar(currentTime.Year));// offset from 1970, since year is stored in uint8_t
}

static void
draw_steps() {
  uint32_t stepCount = sensor.getCounter();
  display.drawBitmap(10, 165, steps, 19, 23, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  display.setCursor(35, 190);
  display.println(stepCount);
}

static void
draw_battery() {
  display.drawBitmap(154, 73, battery, 37, 21, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  display.fillRect(159, 78, 27, BATTERY_SEGMENT_HEIGHT, DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);//clear battery segments
  int8_t batteryLevel = 0;
  float VBAT = get_battery_voltage();
  if (VBAT > BATTERY_SEG_FULL) {
    batteryLevel = 3;
  }
  else if (VBAT > BATTERY_SEG_TWO_THIRD && VBAT <= BATTERY_SEG_FULL) {
    batteryLevel = 2;
  }
  else if (VBAT > BATTERY_SEG_ONE_THIRD && VBAT <= BATTERY_SEG_TWO_THIRD) {
    batteryLevel = 1;
  }
  else if (VBAT <= BATTERY_SEG_ONE_THIRD) {
    batteryLevel = 0;
  }

  for (int8_t batterySegments = 0;
       batterySegments < batteryLevel;
       batterySegments++) {
    display.fillRect(159 + (batterySegments * BATTERY_SEGMENT_SPACING), 78,
		     BATTERY_SEGMENT_WIDTH, BATTERY_SEGMENT_HEIGHT,
		     DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  }
}

void
draw_main_face(void) {
  display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.setTextColor(DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  draw_time();
  draw_date();
  draw_steps();
  draw_battery();
  display.drawBitmap(120, 77, WIFI_CONFIGURED ? wifi : wifioff,
		     26, 18, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  if (BLE_CONFIGURED) {
    display.drawBitmap(100, 75, bluetooth, 13, 21,
		       DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  }
}
