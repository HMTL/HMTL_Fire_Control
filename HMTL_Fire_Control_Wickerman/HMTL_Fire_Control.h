/*******************************************************************************
 * Author: Adam Phelps
 * License: Create Commons Attribution-Non-Commercial
 * Copyright: 2014
 ******************************************************************************/

#ifndef HMTL_FIRE_CONTROL_H
#define HMTL_FIRE_CONTROL_H

#include "RS485Utils.h"
#include "MPR121.h"
#include "PixelUtil.h"
//#include <LiquidTWI.h>
#include "LiquidCrystal.h"

#define BAUD 57600

// LCD display
extern LiquidCrystal lcd;
void update_lcd();
void initialize_display();

// Pixels used for under lighting
extern PixelUtil pixels;

extern config_hdr_t config;
extern output_hdr_t *outputs[HMTL_MAX_OUTPUTS];
extern void *objects[HMTL_MAX_OUTPUTS];

/***** Poofer bullshit ********************************************************/
void update_poofers();

/***** Sensor info ************************************************************/

/* All sensor info is recorded in a bit mask */
extern uint32_t sensor_state;

// XXX - Sensor macros go here?
#define SWITCH_PIN_1 5
#define SWITCH_PIN_2 6
#define SWITCH_PIN_3 9
#define SWITCH_PIN_4 10

void initialize_switches();
void sensor_switches();

extern MPR121 touch_sensor;
void sensor_cap();

void handle_sensors();

// Capacitive sensors
#define POOFER1_QUICK_POOF_SENSOR   3
#define POOFER1_LONG_POOF_SENSOR    2
#define POOFER2_QUICK_POOF_SENSOR   0
#define POOFER2_LONG_POOF_SENSOR    1

#define SENSOR_EXTERNAL_1           4
#define SENSOR_EXTERNAL_2           5
#define SENSOR_EXTERNAL_3           6
#define SENSOR_EXTERNAL_4           7

#define SENSOR_LCD_LEFT             9
#define SENSOR_LCD_UP               8
#define SENSOR_LCD_DOWN            10
#define SENSOR_BOTTOM              11

// Rocker switches
#define POOFER1_IGNITER_SWITCH 0
#define POOFER1_PILOT_SWITCH   1
#define POOFER1_ENABLE_SWITCH  2
#define LIGHTS_ON_SWITCH  3

/***** Connectivity ***********************************************************/

extern RS485Socket rs485;
extern uint16_t my_address;
extern byte *send_buffer;

#define SEND_DATA_SIZE (sizeof (msg_hdr_t) + sizeof (msg_max_t) + 16)
#define SEND_BUFFER_SIZE RS485_BUFFER_TOTAL(SEND_DATA_SIZE) // XXX: Could this be smaller?

// Poofer definitions
#ifndef POOFER1_ADDRESS
  #define POOFER1_ADDRESS  0x41
#endif
#define POOFER1_IGNITER  0x0
#define POOFER1_PILOT    0x1
#define POOFER1_POOF1    0x2
#define POOFER1_POOF2    0x3

#ifndef LIGHTS_ADDRESS
  #define LIGHTS_ADDRESS  0x40
#endif
#define LIGHTS1 0x0
#define LIGHTS2 0x1

extern uint16_t poofer_address;
extern uint16_t lights_address;



void initialize_connect();

void sendHMTLValue(uint16_t address, uint8_t output, int value);
void sendHMTLTimedChange(uint16_t address, uint8_t output,
			 uint32_t change_period,
			 uint32_t start_color,
			 uint32_t stop_color);
void sendHMTLCancel(uint16_t address, uint8_t output);
#endif
