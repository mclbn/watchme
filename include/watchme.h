#ifndef WATCHME_H
#define WATCHME_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <GxEPD2_BW.h>
#include <Wire.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "bma.h"
#include "constants.h"
#include "DSEG7_Classic_Bold_53.h"
#include "WatchyRTC.h"
#include "motor.h"
#include "main_face.h"
#include "mini_face.h"
#include "vcard.h"
#include "wifi.h"
#include "menu_manual_time.h"
#include "menu_ntp_time.h"
#include "menu_battery.h"
#include "menu_stopwatch.h"
#include "menu_groceries.h"

//temp includes, please cleanup
#include "ntp_time.h"

typedef enum {
	FACE_MINI,
	FACE_MAIN,
	FACE_VCARD,
	MAX_FACES
} face_id;

typedef enum {
	STATE_FACE,
	STATE_MENU,
	STATE_MENU_APP,
	MAX_STATES
} state_id;

typedef void (*face_ptr)(void);

void watchme_init(void);
float getBatteryVoltage(void);

// stuff to isolate in dedicated cpp/header files
void show_menu(byte menu_index, bool partialRefresh);

extern GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display;
extern tmElements_t currentTime;
extern WatchyRTC RTC;
extern RTC_DATA_ATTR int gui_state;
extern RTC_DATA_ATTR int menu_index;
extern RTC_DATA_ATTR BMA423 sensor;
extern RTC_DATA_ATTR bool WIFI_CONFIGURED;
extern RTC_DATA_ATTR bool BLE_CONFIGURED;

#endif // WATCHME_H
