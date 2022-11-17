#ifndef CONSTANTS_H
#define CONSTANTS_H

#define DARKMODE false

// Pins
#define SDA 21
#define SCL 22
#define V10_ADC_PIN 33 //OG v1.0 Watchy
#define V15_ADC_PIN 35 //v1.5 Watchy with PCF8563 RTC
#define RTC_PIN GPIO_NUM_27
#define DISPLAY_CS 5
#define DISPLAY_DC 10
#define DISPLAY_RESET 9
#define DISPLAY_BUSY 19
#define VIB_MOTOR_PIN 13
#define MENU_BTN_PIN 26
#define BACK_BTN_PIN 25
#define UP_BTN_PIN 32
#define DOWN_BTN_PIN 4
#define MENU_BTN_MASK GPIO_SEL_26
#define BACK_BTN_MASK GPIO_SEL_25
#define UP_BTN_MASK GPIO_SEL_32
#define DOWN_BTN_MASK GPIO_SEL_4
#define ACC_INT_MASK GPIO_SEL_14
#define BTN_PIN_MASK MENU_BTN_MASK|BACK_BTN_MASK|UP_BTN_MASK|DOWN_BTN_MASK // |ACC_INT_MASK

// Display
#define DISPLAY_WIDTH 200
#define DISPLAY_HEIGHT 200

// Menu
#define MENU_HEIGHT 30
#define MENU_LENGTH 6

#define MENU_BATTERY "Battery status"
#define MENU_NTP_TIME "Get NTP time"
#define MENU_STOPWATCH "Stopwatch"
#define MENU_TIME "Set time"
#define MENU_GROCERIES "Groceries"
#define MENU_RESET "Reset"

#define MENU_TIMEOUT (1000 * 30)

// WiFi
#define WIFI_AP_TIMEOUT 60
#define WIFI_AP_SSID "Watchme AP"

// Time Setup
#define SET_HOUR 0
#define SET_MINUTE 1
#define SET_YEAR 2
#define SET_MONTH 3
#define SET_DAY 4
#define YEAR_OFFSET 2000
#define HOUR_12_24 24

// GMT +1 = 3600
// GMT +8 = 28800
// GMT -1 = -3600
// GMT 0 = 0
// We Set to UTC
// because we will handle Daytime savings with Timezone.h parameters
#define TIME_OFFSET (0)
#define NTP_SERVER "pool.ntp.org"

// StopWatch
#define MENU_STOPWATCH_TIMEOUT (1000 * 60 * 1)

#endif // CONSTANTS_H
