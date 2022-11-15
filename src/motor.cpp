#include "motor.h"

void
vibrate_motor(uint8_t intervalMs, uint8_t length) {
  pinMode(VIB_MOTOR_PIN, OUTPUT);

  for (int i = 0; i < length; i++) {
    digitalWrite(VIB_MOTOR_PIN, true);
    delay(intervalMs);
    digitalWrite(VIB_MOTOR_PIN, false);
    delay(intervalMs);
  }
}
