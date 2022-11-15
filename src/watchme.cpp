#include "watchme.h"

WatchyRTC RTC;

GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(CS,
									 DC,
									 RESET,
									 BUSY));

tmElements_t currentTime;

face_ptr faces[MAX_FACES] = {
  &draw_mini_face,
  &draw_main_face,
  &draw_vcard
};

RTC_DATA_ATTR face_id current_face = FACE_MINI;
RTC_DATA_ATTR int gui_state;
RTC_DATA_ATTR int menu_index;
RTC_DATA_ATTR BMA423 sensor;
RTC_DATA_ATTR bool WIFI_CONFIGURED;
RTC_DATA_ATTR bool BLE_CONFIGURED;
RTC_DATA_ATTR weatherData currentWeather;
RTC_DATA_ATTR int weatherIntervalCounter = WEATHER_UPDATE_INTERVAL;

static void
reset_watch(void) {
  ESP.restart();
}

static void
deep_sleep() {
  esp_sleep_enable_ext0_wakeup(RTC_PIN, 0); //enable deep sleep wake on RTC interrupt
  esp_sleep_enable_ext1_wakeup(BTN_PIN_MASK, ESP_EXT1_WAKEUP_ANY_HIGH); //enable deep sleep wake on button press
  esp_deep_sleep_start();
}

static uint16_t
_readRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)address, (uint8_t)len);
  uint8_t i = 0;
  while (Wire.available()) {
    data[i++] = Wire.read();
  }
  return 0;
}

static uint16_t
_writeRegister(uint8_t address, uint8_t reg, uint8_t *data, uint16_t len) {
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(data, len);
  return (0 != Wire.endTransmission());
}

static void
_bmaConfig() {
  if (sensor.begin(_readRegister, _writeRegister, delay) == false) {
    //fail to init BMA
    return;
  }

  // Accel parameter structure
  Acfg cfg;
  /*!
    Output data rate in Hz, Optional parameters:
    - BMA4_OUTPUT_DATA_RATE_0_78HZ
    - BMA4_OUTPUT_DATA_RATE_1_56HZ
    - BMA4_OUTPUT_DATA_RATE_3_12HZ
    - BMA4_OUTPUT_DATA_RATE_6_25HZ
    - BMA4_OUTPUT_DATA_RATE_12_5HZ
    - BMA4_OUTPUT_DATA_RATE_25HZ
    - BMA4_OUTPUT_DATA_RATE_50HZ
    - BMA4_OUTPUT_DATA_RATE_100HZ
    - BMA4_OUTPUT_DATA_RATE_200HZ
    - BMA4_OUTPUT_DATA_RATE_400HZ
    - BMA4_OUTPUT_DATA_RATE_800HZ
    - BMA4_OUTPUT_DATA_RATE_1600HZ
  */
  cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
  /*!
    G-range, Optional parameters:
    - BMA4_ACCEL_RANGE_2G
    - BMA4_ACCEL_RANGE_4G
    - BMA4_ACCEL_RANGE_8G
    - BMA4_ACCEL_RANGE_16G
  */
  cfg.range = BMA4_ACCEL_RANGE_2G;
  /*!
    Bandwidth parameter, determines filter configuration, Optional parameters:
    - BMA4_ACCEL_OSR4_AVG1
    - BMA4_ACCEL_OSR2_AVG2
    - BMA4_ACCEL_NORMAL_AVG4
    - BMA4_ACCEL_CIC_AVG8
    - BMA4_ACCEL_RES_AVG16
    - BMA4_ACCEL_RES_AVG32
    - BMA4_ACCEL_RES_AVG64
    - BMA4_ACCEL_RES_AVG128
  */
  cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;

  /*! Filter performance mode , Optional parameters:
    - BMA4_CIC_AVG_MODE
    - BMA4_CONTINUOUS_MODE
  */
  cfg.perf_mode = BMA4_CONTINUOUS_MODE;

  // Configure the BMA423 accelerometer
  sensor.setAccelConfig(cfg);

  // Enable BMA423 accelerometer
  // Warning : Need to use feature, you must first enable the accelerometer
  // Warning : Need to use feature, you must first enable the accelerometer
  sensor.enableAccel();

  struct bma4_int_pin_config config ;
  config.edge_ctrl = BMA4_LEVEL_TRIGGER;
  config.lvl = BMA4_ACTIVE_HIGH;
  config.od = BMA4_PUSH_PULL;
  config.output_en = BMA4_OUTPUT_ENABLE;
  config.input_en = BMA4_INPUT_DISABLE;
  // The correct trigger interrupt needs to be configured as needed
  sensor.setINTPinConfig(config, BMA4_INTR1_MAP);

  struct bma423_axes_remap remap_data;
  remap_data.x_axis = 1;
  remap_data.x_axis_sign = 0xFF;
  remap_data.y_axis = 0;
  remap_data.y_axis_sign = 0xFF;
  remap_data.z_axis = 2;
  remap_data.z_axis_sign = 0xFF;
  // Need to raise the wrist function, need to set the correct axis
  sensor.setRemapAxes(&remap_data);

  // Enable BMA423 isStepCounter feature
  sensor.enableFeature(BMA423_STEP_CNTR, true);
  // Enable BMA423 isTilt feature
  //  sensor.enableFeature(BMA423_TILT, true);
  // Enable BMA423 isDoubleClick feature
  //  sensor.enableFeature(BMA423_WAKEUP, true);
  // Enable BMA423 activity feature
  //  sensor.enableFeature(BMA423_ACTIVITY, true);

  // Reset steps
  sensor.resetStepCounter();

  // Turn on feature interrupt
  sensor.enableStepCountInterrupt();
  //  sensor.enableTiltInterrupt();
  // It corresponds to isDoubleClick interrupt
  //  sensor.enableWakeupInterrupt();
  // Activity interrupt
  //  sensor.enableActivityInterrupt();
}

void
show_menu(byte menu_index, bool partialRefresh) {
  display.init(0, false); //_initial_refresh to false to prevent full update on init
  display.setFullWindow();
  display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.setFont(&FreeMonoBold9pt7b);

  int16_t  x1, y1;
  uint16_t w, h;
  int16_t yPos;

  const char * menuItems[] = {
    MENU_STOPWATCH,
    MENU_BATTERY,
    MENU_NTP_TIME,
    MENU_TIME,
    MENU_GROCERIES,
    MENU_RESET
  };

  for (int i = 0; i < MENU_LENGTH; i++) {
    yPos = 30+(MENU_HEIGHT*i);
    display.setCursor(0, yPos);
    if (i == menu_index) {
      display.getTextBounds(menuItems[i], 0, yPos, &x1, &y1, &w, &h);
      display.fillRect(x1 - 1, y1 - 10, 200, h + 15,
		       DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
      display.setTextColor(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
      display.println(menuItems[i]);
    } else {
      display.setTextColor(DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
      display.println(menuItems[i]);
    }
  }

  display.display(partialRefresh);
  //display.hibernate();

  gui_state = STATE_MENU;
}

static void
show_fast_menu(byte menu_index) {
  display.setFullWindow();
  display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.setFont(&FreeMonoBold9pt7b);

  int16_t  x1, y1;
  uint16_t w, h;
  int16_t yPos;

  const char * menuItems[] = {
    MENU_STOPWATCH,
    MENU_BATTERY,
    MENU_NTP_TIME,
    MENU_TIME,
    MENU_GROCERIES,
    MENU_RESET
  };

  for (int i = 0; i < MENU_LENGTH; i++) {
    yPos = 30 + (MENU_HEIGHT * i);
    display.setCursor(0, yPos);
    if (i == menu_index) {
      display.getTextBounds(menuItems[i], 0, yPos, &x1, &y1, &w, &h);
      display.fillRect(x1 - 1, y1 - 10, 200, h + 15,
		       DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
      display.setTextColor(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
      display.println(menuItems[i]);
    } else {
      display.setTextColor(DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
      display.println(menuItems[i]);
    }
  }

  display.display(true);

  gui_state = STATE_MENU;
}

static void
showAccelerometer() {
  display.init(0, true); //_initial_refresh to false to prevent full update on init
  display.setFullWindow();
  display.fillScreen(GxEPD_BLACK);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_WHITE);

  Accel acc;

  long previousMillis = 0;
  long interval = 200;

  gui_state = STATE_MENU_APP;

  pinMode(BACK_BTN_PIN, INPUT);

  while(1) {

    unsigned long currentMillis = millis();

    if (digitalRead(BACK_BTN_PIN) == 1) {
      break;
    }

    if (currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;
      // Get acceleration data
      bool res = sensor.getAccel(acc);
      uint8_t direction = sensor.getDirection();
      display.fillScreen(GxEPD_BLACK);
      display.setCursor(0, 30);
      if (res == false) {
	display.println("getAccel FAIL");
      } else {
        display.print("  X:"); display.println(acc.x);
        display.print("  Y:"); display.println(acc.y);
        display.print("  Z:"); display.println(acc.z);

        display.setCursor(30, 130);
        switch(direction) {
	case DIRECTION_DISP_DOWN:
	  display.println("FACE DOWN");
	  break;
	case DIRECTION_DISP_UP:
	  display.println("FACE UP");
	  break;
	case DIRECTION_BOTTOM_EDGE:
	  display.println("BOTTOM EDGE");
	  break;
	case DIRECTION_TOP_EDGE:
	  display.println("TOP EDGE");
	  break;
	case DIRECTION_RIGHT_EDGE:
	  display.println("RIGHT EDGE");
	  break;
	case DIRECTION_LEFT_EDGE:
	  display.println("LEFT EDGE");
	  break;
	default:
	  display.println("ERROR!!!");
	  break;
        }

      }
      display.display(true); //full refresh
    }
  }

  show_menu(menu_index, false);
}

void
update_weather_data(void) {
  if (connectWiFi()) { //Use Weather API for live data if WiFi is connected
    HTTPClient http;
    http.setConnectTimeout(3000); //3 second max timeout
    String weatherQueryURL = String(OPENWEATHERMAP_URL)
      + String(CITY_NAME) + String(",")
      + String(COUNTRY_CODE) + String("&units=")
      + String(TEMP_UNIT) + String("&appid=")
      + String(OPENWEATHERMAP_APIKEY);
    http.begin(weatherQueryURL.c_str());
    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
      String payload = http.getString();
      JSONVar responseObject = JSON.parse(payload);
      currentWeather.temperature = int(responseObject["main"]["temp"]);
      currentWeather.weatherConditionCode = int(responseObject["weather"][0]["id"]);
    } else {
      //http error
    }
    http.end();
    //turn off radios
    WiFi.mode(WIFI_OFF);
    // Are we sure about that ? Isn't btStop fow BT ?
    btStop();
  } else {
    //No WiFi, no update!
  }
}

static void
show_watch_face(bool partial_refresh) {
  display.init(0, false); //_initial_refresh to false to prevent full update on init
  display.setFullWindow();
  faces[current_face]();
  display.display(partial_refresh); //partial refresh
  display.hibernate();
  gui_state = STATE_FACE;
}

static void
handle_button_press(void) {
  uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();

  //  Serial.printf("Interrupt: %" PRIu64 "\n", wakeupBit);

  //Menu Button
  if (wakeupBit & MENU_BTN_MASK) {
    if (gui_state == STATE_FACE) { //enter menu state if coming from watch face
      show_menu(menu_index, false);
    } else if (gui_state == STATE_MENU) { //if already in menu, then select menu item
      switch(menu_index) {
      case 0:
	menu_stopwatch();
	break;
      case 1:
	menu_battery_status();
	break;
      case 2:
	menu_ntp_time();
	break;
      case 3:
	menu_set_time();
	break;
      case 4:
	menu_groceries();
	break;
      case 5:
	reset_watch();
	break;
      default:
	break;
      }
    }
  }

  //Back Button
  else if (wakeupBit & BACK_BTN_MASK) {
    if (gui_state == STATE_MENU) { //exit to watch face if already in menu
      RTC.clearAlarm(); //resets the alarm flag in the RTC
      RTC.read(currentTime);
      show_watch_face(false);
    } else if (gui_state == STATE_MENU_APP) {
      show_menu(menu_index, false);//exit to menu if already in app
    } else if (gui_state == STATE_FACE) {
      return;
    }
  }

  //Up Button
  else if (wakeupBit & UP_BTN_MASK) {
    if (gui_state == STATE_MENU) { //increment menu index
      menu_index--;
      if (menu_index < 0) {
        menu_index = MENU_LENGTH - 1;
      }
      show_menu(menu_index, true);
    } else if (gui_state == STATE_FACE) {
      switch (current_face) {
      case FACE_MINI:
	current_face = FACE_VCARD;
	show_watch_face(false);
	return;
      case FACE_MAIN:
	RTC.read(currentTime);
	current_face = FACE_MINI;
	show_watch_face(false);
	return;
      case FACE_VCARD:
	RTC.read(currentTime);
	current_face = FACE_MAIN;
	show_watch_face(false);
	return;
      default:
	break;
      }
    }
  }

  //Down Button
  else if (wakeupBit & DOWN_BTN_MASK) {
    if (gui_state == STATE_MENU) { //decrement menu index
      menu_index++;
      if (menu_index > MENU_LENGTH - 1) {
        menu_index = 0;
      }
      show_menu(menu_index, true);
    } else if (gui_state == STATE_FACE) {
      switch (current_face) {
      case FACE_MINI:
	RTC.read(currentTime);
	current_face = FACE_MAIN;
	show_watch_face(false);
	return;
      case FACE_MAIN:
	current_face = FACE_VCARD;
	show_watch_face(false);
	return;
      case FACE_VCARD:
	RTC.read(currentTime);
	current_face = FACE_MINI;
	show_watch_face(false);
	return;
      default:
	break;
      }
    }
  }

  /***************** fast menu *****************/
  bool timeout = false;
  long lastTimeout = millis();

  pinMode(MENU_BTN_PIN, INPUT);
  pinMode(BACK_BTN_PIN, INPUT);
  pinMode(UP_BTN_PIN, INPUT);
  pinMode(DOWN_BTN_PIN, INPUT);
  while (!timeout) {
    if (millis() - lastTimeout > MENU_TIMEOUT) {
      timeout = true;
    } else {
      if (digitalRead(MENU_BTN_PIN) == 1) {
	lastTimeout = millis();
	if (gui_state == STATE_MENU) { //if already in menu, then select menu item
	  switch (menu_index) {
	  case 0:
	    menu_stopwatch();
	    break;
	  case 1:
	    menu_battery_status();
	    break;
	  case 2:
	    menu_ntp_time();
	    break;
	  case 3:
	    menu_set_time();
	    break;
	  case 4:
	    menu_groceries();
	    break;
	  case 5:
	    reset_watch();
	    break;
	  default:
	    break;
	  }
	  lastTimeout = millis();
	}
      } else if (digitalRead(BACK_BTN_PIN) == 1) {
	lastTimeout = millis();
	if (gui_state == STATE_MENU) { //exit to watch face if already in menu
	  RTC.clearAlarm(); //resets the alarm flag in the RTC
	  RTC.read(currentTime);
	  show_watch_face(false);
	  break; //leave loop
	} else if (gui_state == STATE_MENU_APP) {
	  show_menu(menu_index, false);//exit to menu if already in app
	}
      } else if (digitalRead(UP_BTN_PIN) == 1) {
	lastTimeout = millis();
	if (gui_state == STATE_MENU) { //increment menu index
	  menu_index--;
	  if (menu_index < 0) {
	    menu_index = MENU_LENGTH - 1;
	  }
	  show_fast_menu(menu_index);
	}
      } else if (digitalRead(DOWN_BTN_PIN) == 1) {
	lastTimeout = millis();
	if (gui_state == STATE_MENU) { //decrement menu index
	  menu_index++;
	  if (menu_index > MENU_LENGTH - 1) {
	    menu_index = 0;
	  }
	  show_fast_menu(menu_index);
	}
      }
    }
  }
  RTC.clearAlarm(); //resets the alarm flag in the RTC
  RTC.read(currentTime);
  show_watch_face(false);
}

static void
handle_custom_events(void) {
  bool time_update_success = true;

  if (currentTime.Hour == 0 && currentTime.Minute == 0) {
    // reset step counter at midnight
    sensor.resetStepCounter();
  }

  if (currentTime.Hour == 7 // Update time every 2 days through NTP
      && currentTime.Minute == 0 // or next day if last update failed
      && ((currentTime.Day & 1) || time_update_success == false)) {
    time_update_success = false;
    // A nice change would be to store last update day to only update
    // 2 days after last one

    time_t ntp_time = get_ntp_time();
    if (ntp_time != 0) {
      update_rtc_from_ntp(ntp_time);
      time_update_success = true;
    }
    // must update after daytime savings change (last sunday of march and october)
      // any sunday past 24 (excluded) is the last sunday for march or october
  }

  if (currentTime.Minute == 0) { // quick vibrate every hour
    vibrate_motor(100, 1);
  }

  if (current_face == FACE_MAIN) {
    // Only update weather if current face requires it
    if (weatherIntervalCounter >= WEATHER_UPDATE_INTERVAL) {
      //only update if WEATHER_UPDATE_INTERVAL has elapsed (i.e. 30 minutes)
      update_weather_data();
      weatherIntervalCounter = 0;
    }
  }
  weatherIntervalCounter++;
}

void
watchme_init(void) {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause(); //get wake up reason
  Wire.begin(SDA, SCL); //init i2c
  RTC.init();

  switch (wakeup_reason) {
  case ESP_SLEEP_WAKEUP_EXT0: //RTC Alarm
    RTC.clearAlarm(); //resets the alarm flag in the RTC
    if (gui_state == STATE_FACE) {
      RTC.read(currentTime);
      handle_custom_events();
      show_watch_face(true); //partial updates on tick
    }
    break;
  case ESP_SLEEP_WAKEUP_EXT1: //button Press
    handle_button_press();
    break;
  default: //reset
    RTC.config("");
    _bmaConfig();
    RTC.read(currentTime);
    check_wireless_interfaces();
    show_watch_face(false); //full update on reset
    break;
  }
  deep_sleep();
}
