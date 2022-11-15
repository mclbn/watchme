#include "battery.h"

float
get_battery_voltage(void) {
  if (RTC.rtcType == DS3231)
    return analogReadMilliVolts(V10_ADC_PIN) / 1000.0f * 2.0f; // Battery voltage goes through a 1/2 divider.
  else
    return analogReadMilliVolts(V15_ADC_PIN) / 1000.0f * 2.0f;
}
