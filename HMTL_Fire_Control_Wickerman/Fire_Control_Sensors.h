/*******************************************************************************
 * Author: Adam Phelps
 * License: Create Commons Attribution-Non-Commercial
 * Copyright: 2016
 ******************************************************************************/

#ifndef FIRE_CONTROL_SENSORS_H
#define FIRE_CONTROL_SENSORS_H

#include "Arduino.h"

#define DISPLAY_CAP_SENSORS   0
#define DISPLAY_ADJUST_LEFT1  1
#define DISPLAY_ADJUST_LEFT2  2
#define DISPLAY_ADJUST_RIGHT1 3
#define DISPLAY_ADJUST_RIGHT2 4
#define DISPLAY_ADJUST_BRIGHTNESS 5
#define DISPLAY_LED_MODE 6
#define DISPLAY_ADDRESS_MODE 7
#define DISPLAY_MAX (7 + 1)

extern uint8_t display_mode;
#define NUM_DISPLAY_MODES DISPLAY_MAX

#define LED_MODE_ON    0
#define LED_MODE_BLINK 1
#define LED_MODE_MAX (1 + 1)
extern uint8_t led_mode;
extern uint8_t led_mode_value;

byte sensor_to_led(byte sensor);

#endif
