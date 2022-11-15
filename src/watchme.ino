#include "watchme.h"

void
setup() {
  Serial.begin(115200);
  Serial.println("Debug is on");
  watchme_init();
}

void
loop() {
}
