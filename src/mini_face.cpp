#include "mini_face.h"

static void
draw_hand(GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> &display,
          int16_t len, float angle, float_t hand_width, uint16_t hand_color)
{
  static const int16_t xc = GxEPD2_154_D67::WIDTH / 2;
  static const int16_t yc = GxEPD2_154_D67::HEIGHT / 2;

  int16_t xe = floor(-len * sin(angle));
  int16_t ye = floor(-len * cos(angle));

  const int16_t xb = round(-ye / (len / hand_width));
  const int16_t yb = round(xe / (len / hand_width));

  xe += xc;
  ye += yc;

  display.fillTriangle(xc - xb, yc - yb, xc + xb,
                       yc + yb, xe + xb, ye + yb,
                       hand_color);
  display.fillTriangle(xc - xb, yc - yb, xe + xb,
                       ye + yb, xe - xb, ye - yb,
                       hand_color);
}

void
draw_mini_face(void) {
  unsigned int hours;
  unsigned int minutes;

  const uint16_t hand_color = DARKMODE ? GxEPD_WHITE : GxEPD_BLACK;

  display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.setTextColor(DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);

  hours = ((currentTime.Hour + 11) % 12) + 1;
  minutes = currentTime.Minute;

  draw_hand(display, 60, (hours + minutes / 60.0) / 12.0 * -TWO_PI,
            HAND_WIDTH, hand_color);
  draw_hand(display, 90, minutes / 60.0 * -TWO_PI,
            HAND_WIDTH, hand_color);

  static const int16_t inner_tick_lenght = GxEPD2_154_D67::WIDTH / 20;
  static const int16_t center_x = GxEPD2_154_D67::WIDTH / 2;
  static const int16_t center_y = GxEPD2_154_D67::HEIGHT / 2;
  static const int16_t inner_tick_radius = center_x - inner_tick_lenght;
  static const int16_t outer_tick_radius = center_x;

  // Draw the ticks on the sides of the screen
  for (unsigned int tick_index = 0; tick_index < 12; tick_index++) {
    unsigned int offset;

    const float_t tick_rotation = tick_index * TWO_PI / 12;
    const float_t tick_sin = sin(tick_rotation);
    const float_t tick_cos = cos(tick_rotation);
    const float_t inner_x = tick_sin * (inner_tick_radius + offset);
    const float_t inner_y = -tick_cos * (inner_tick_radius + offset);
    const float_t outer_x = tick_sin * (outer_tick_radius + offset);
    const float_t outer_y = -tick_cos * (outer_tick_radius + offset);

    display.drawLine(center_x + inner_x,
                     center_y + inner_y,
                     center_x + outer_x,
                     center_y + outer_y,
                     hand_color);
  }
}
