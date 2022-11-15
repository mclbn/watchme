#ifndef MAIN_FACE_H
#define MAIN_FACE_H

#include "watchme.h"
#include "Seven_Segment10pt7b.h"
#include "DSEG7_Classic_Regular_15.h"
#include "DSEG7_Classic_Bold_25.h"
#include "DSEG7_Classic_Regular_39.h"
#include "icons.h"
#include "battery.h"

#define BATTERY_SEG_FULL (4.1)
#define BATTERY_SEG_TWO_THIRD (3.5)
#define BATTERY_SEG_ONE_THIRD (3.0)

void draw_main_face(void);

#endif // MAIN_FACE_H
