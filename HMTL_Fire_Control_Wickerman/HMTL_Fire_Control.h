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
#include "TimeSync.h"

#define BAUD 115200

/*
 * Definitions for the different controllers that work with this code
 */
#define OBJECT_TYPE_FIRE_CONTROLLER 4
#define OBJECT_TYPE_TOUCH_CONTROLLER 5

#ifndef OBJECT_TYPE
  #define OBJECT_TYPE OBJECT_TYPE_FIRE_CONTROLLER
#endif

/*
 * Control modes for specific poofer configurations
 */
#define CONTROL_SINGLE_DOUBLE 1 // Single ignitor/pilot with two accumulators.
#define CONTROL_SINGLE_QUAD   2 // Single ignitor/pilot with four accumulators.
#define CONTROL_DOUBLE_DOUBLE 3 // Two devices, each with single ignitor/pilot
                                // and two accumulators.
#define CONTROL_SINGLE_QUINT  4 // One device w/ two controllers for five accumulators, one igniter,
                                // and one pilot, plus optional HMTL controller for lights.

#ifndef CONTROL_MODE
  #define CONTROL_MODE CONTROL_SINGLE_DOUBLE
#endif

// TODO: Remove before commit
#define OBJECT_TYPE OBJECT_TYPE_TOUCH_CONTROLLER  // TODO: Remove before commit
#define CONTROL_MODE CONTROL_SINGLE_QUINT         // TODO: Remove before commit
// TODO: Remove before commit


// LCD display
extern LiquidCrystal lcd;
void update_lcd();
void initialize_display();

// Pixels used for under lighting
extern PixelUtil pixels;

extern config_hdr_t config;
extern output_hdr_t *outputs[HMTL_MAX_OUTPUTS];
extern void *objects[HMTL_MAX_OUTPUTS];

extern TimeSync timesync;

/***** Sensor info ************************************************************/

/* All sensor info is recorded in a bit mask */
extern uint32_t sensor_state;

/* Physical pins for the rocker switches */
#define SWITCH_PIN_1 5
#define SWITCH_PIN_2 6
#define SWITCH_PIN_3 9
#define SWITCH_PIN_4 10

void initialize_switches();
void sensor_switches();
void calculate_pulse();

extern MPR121 touch_sensor;
void sensor_cap();

void handle_sensors();

/*
 * Rocker switches
 */
#define POOFER_IGNITER_SWITCH 0
#define POOFER_PILOT_SWITCH   1
#define POOFER_ENABLE_SWITCH  2
#define LIGHTS_ON_SWITCH  3


/*
 * Capacitive sensors configurations
 */

#if OBJECT_TYPE == OBJECT_TYPE_TOUCH_CONTROLLER

  #if CONTROL_MODE == CONTROL_SINGLE_QUINT
    /* Normal mode switches */
    #define POOFER1_QUICK_SENSOR          0
    #define POOFER2_QUICK_SENSOR          1
    #define POOFER3_QUICK_SENSOR          2
    #define POOFER4_QUICK_SENSOR          3
    #define POOFER5_QUICK_SENSOR          4

    #define POOFER1_LONG_SENSOR           6
    #define POOFER2_LONG_SENSOR           7
    #define POOFER3_LONG_SENSOR           8
    #define POOFER4_LONG_SENSOR           9
    #define POOFER5_LONG_SENSOR          10

    #define POOFER_PROGRAM_1_SENSOR       5
    #define POOFER_PROGRAM_2_SENSOR      11

    /* Replace the "lights on" switch with a programming mode switch */
    #define PROGRAM_MODE_SWITCH           3
    #define LIGHTS_ON_SWITCH             -1

    /* Programming mode sensors */
    #define POOFER_PROGRAM_A_SENSOR       0
    // TODO: ...
  #else
    #define POOFER1_POOF2_QUICK_SENSOR    0
    #define POOFER1_POOF2_LONG_SENSOR     1
    #define POOFER1_MODE2_SENSOR          2
    #define POOFER1_MODE1_SENSOR          3
    #define POOFER1_POOF1_LONG_SENSOR     4
    #define POOFER1_POOF1_QUICK_SENSOR    5

    #define POOFER2_POOF2_QUICK_SENSOR    6
    #define POOFER2_POOF2_LONG_SENSOR     7
    #define POOFER2_MODE2_SENSOR          8
    #define POOFER2_MODE1_SENSOR          9
    #define POOFER2_POOF1_LONG_SENSOR    10
    #define POOFER2_POOF1_QUICK_SENSOR   11
  #endif

  #define SENSOR_MENU_ENABLE_1           11
  #define SENSOR_MENU_ENABLE_2            6
  #define SENSOR_LCD_NEXT                10
  #define SENSOR_LCD_UP                   9
  #define SENSOR_LCD_DOWN                 8

  #define SENSOR_DISPLAY_MODE             SENSOR_LCD_NEXT

#elif OBJECT_TYPE == OBJECT_TYPE_FIRE_CONTROLLER
  #define POOFER1_POOF1_QUICK_SENSOR   3
  #define POOFER1_POOF1_LONG_SENSOR    2
  #define POOFER1_POOF2_QUICK_SENSOR   0
  #define POOFER1_POOF2_LONG_SENSOR    1

  #define SENSOR_EXTERNAL_1            4
  #define SENSOR_EXTERNAL_2            5
  #define SENSOR_EXTERNAL_3            6
  #define SENSOR_EXTERNAL_4            7

  #define SENSOR_LCD_LEFT              9
  #define SENSOR_LCD_UP                8
  #define SENSOR_LCD_DOWN             10
  #define SENSOR_BOTTOM               11

  #define SENSOR_DISPLAY_MODE         SENSOR_LCD_LEFT
#endif

/***** Connectivity ***********************************************************/

extern RS485Socket rs485;
extern uint16_t my_address;
extern byte *send_buffer;

#define SEND_DATA_SIZE (sizeof (msg_hdr_t) + sizeof (msg_max_t) + 16)
#define SEND_BUFFER_SIZE RS485_BUFFER_TOTAL(SEND_DATA_SIZE) // XXX: Could this be smaller?

// Poofer definitions
#ifndef POOFER1_ADDRESS
  #define POOFER1_ADDRESS  0x40
#endif
#ifndef POOFER2_ADDRESS
  #define POOFER2_ADDRESS 0x41
#endif

#define POOFER1_IGNITER  0x0
#define POOFER1_PILOT    0x3

#if CONTROL_MODE == CONTROL_SINGLE_QUINT
  #define POOFER1_LARGE    0x2
  #define POOFER1_UNUSED   0x3

  #define POOFER2_POOF1    0x0
  #define POOFER2_POOF2    0x1
  #define POOFER2_POOF3    0x2
  #define POOFER2_POOF4    0x3
#else
  #define POOFER1_POOF1    0x2
  #define POOFER1_POOF2    0x3

  #ifndef POOFER2_ADDRESS
    #define POOFER2_ADDRESS  0x41
  #endif
  #define POOFER2_IGNITER  0x0
  #define POOFER2_PILOT    0x1
  #define POOFER2_POOF1    0x2
  #define POOFER2_POOF2    0x3
#endif

#ifndef LIGHTS_ADDRESS
  #define LIGHTS_ADDRESS  0x45
#endif

extern uint16_t poofer1_address;
extern uint16_t poofer2_address;
extern uint16_t lights_address;

/* Communication prototypes */
void sendHMTLValue(uint16_t address, uint8_t output, int value);
void sendHMTLTimedChange(uint16_t address, uint8_t output,
			 uint32_t change_period,
			 uint32_t start_color,
			 uint32_t stop_color);
void sendHMTLCancel(uint16_t address, uint8_t output);
void sendHMTLBlink(uint16_t address, uint8_t output,
                   uint16_t onperiod, uint32_t oncolor,
                   uint16_t offperiod, uint32_t offcolor);
#endif
