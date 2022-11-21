#include "vcard.h"

void
draw_vcard(void) {
  display.fillScreen(GxEPD_BLACK);
  display.drawBitmap(0, 0, qr_perso,
                     DISPLAY_WIDTH, DISPLAY_HEIGHT, GxEPD_WHITE);
}
