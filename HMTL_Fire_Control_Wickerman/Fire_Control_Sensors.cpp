/*******************************************************************************
 * Author: Adam Phelps
 * License: Create Commons Attribution-Non-Commercial
 * Copyright: 2014
 ******************************************************************************/

#define DEBUG_LEVEL DEBUG_TRACE
#include <Debug.h>

#include <Arduino.h>
#include <NewPing.h>
#include <Wire.h>
#include "MPR121.h"

#include "HMTLTypes.h"
#include "HMTLMessaging.h"
#include "HMTLPoofer.h"

#include "HMTL_Fire_Control.h"

boolean data_changed = true;

/******* Switches *************************************************************/

#define NUM_SWITCHES 4
boolean switch_states[NUM_SWITCHES] = { false, false, false, false };
boolean switch_changed[NUM_SWITCHES] = { false, false, false, false };
const byte switch_pins[NUM_SWITCHES] = {
  SWITCH_PIN_1, SWITCH_PIN_2, SWITCH_PIN_3, SWITCH_PIN_4 };

void initialize_switches(void) {
  for (byte i = 0; i < NUM_SWITCHES; i++) {
    pinMode(switch_pins[i], INPUT);
  }
}

void sensor_switches(void) {
  for (byte i = 0; i < NUM_SWITCHES; i++) {
    boolean value = (digitalRead(switch_pins[i]) == LOW);
    if (value != switch_states[i]) {
      switch_changed[i] = true;
      data_changed = true;
      switch_states[i] = value;
      if (value) {
        DEBUG5_VALUELN("Switch is on: ", i);
      } else {
        DEBUG5_VALUELN("Switch is off: ", i);
      }
    } else {
      switch_changed[i] = false;
    }
  }
}

/******* Capacitive Sensors ***************************************************/

void sensor_cap(void) 
{
  if (touch_sensor.readTouchInputs()) {
    DEBUG_COMMAND(DEBUG_TRACE,
                  DEBUG5_PRINT("Cap:");
                  for (byte i = 0; i < MPR121::MAX_SENSORS; i++) {
                    DEBUG5_VALUE(" ", touch_sensor.touched(i));
                  }
                  DEBUG5_VALUELN(" ms:", millis());
                  );
    data_changed = true;
  }
}

/******* Handle Sensors *******************************************************/


byte display_mode = 1;
#define NUM_DISPLAY_MODES 2

void sendOn(uint16_t address, uint8_t output) {
  sendHMTLValue(address, output, 255);
}

void sendOff(uint16_t address, uint8_t output) {
  sendHMTLValue(address, output, 0);
}

void sendBurst(uint16_t address, uint8_t output, uint32_t duration) {
  sendHMTLTimedChange(address,
                      output, duration, 0xFFFFFFFF, 0);
}

void sendCancel(uint16_t address, uint8_t output) {
  sendHMTLCancel(address, output);
}

void handle_sensors(void) {
  static unsigned long last_send = millis();

  /* Igniter switches */
  if (switch_changed[POOFER1_IGNITER_SWITCH]) {
    if (switch_states[POOFER1_IGNITER_SWITCH]) {
      DEBUG1_PRINTLN("IGNITE ON");
      sendOn(SOCKET_ADDR_ANY, POOFER1_IGNITER);
    } else {
      DEBUG1_PRINTLN("IGNITE OFF");
      sendOff(SOCKET_ADDR_ANY, POOFER1_IGNITER);
    }
  }

  /* Pilot Switch */
  if (switch_changed[POOFER1_PILOT_SWITCH]) {
    if (switch_states[POOFER1_PILOT_SWITCH]) {
      DEBUG1_PRINTLN("PILOT ON");
      sendOn(SOCKET_ADDR_ANY, POOFER1_PILOT);
    } else {
      DEBUG1_PRINTLN("PILOT OFF");
      sendOff(SOCKET_ADDR_ANY, POOFER1_PILOT);
    }
  }

  // Poofer Enable/Disable
  if (switch_changed[POOFER1_ENABLE_SWITCH]) {
    if (switch_states[POOFER1_ENABLE_SWITCH]) {
      DEBUG1_PRINTLN("POOFERS ENABLED");
    } else {
      DEBUG1_PRINTLN("POOFERS DISABLED");
      sendCancel(SOCKET_ADDR_ANY, POOFER1_POOF1);
      sendCancel(SOCKET_ADDR_ANY, POOFER1_POOF2);

      sendOff(SOCKET_ADDR_ANY, POOFER1_POOF1);
      sendOff(SOCKET_ADDR_ANY, POOFER1_POOF2);
    }
  }

  if (switch_states[POOFER1_ENABLE_SWITCH] &&
          switch_states[POOFER1_PILOT_SWITCH]) {
    /* Poofer controls */
    if (touch_sensor.changed(POOFER1_QUICK_POOF_SENSOR) &&
        touch_sensor.touched(POOFER1_QUICK_POOF_SENSOR)) {
      sendBurst(SOCKET_ADDR_ANY, POOFER1_POOF1, 50);
    }

    if (touch_sensor.touched(POOFER1_LONG_POOF_SENSOR)) {
      sendBurst(SOCKET_ADDR_ANY, POOFER1_POOF1, 250);
    } else if (touch_sensor.changed(POOFER1_LONG_POOF_SENSOR)) {
      sendOff(SOCKET_ADDR_ANY, POOFER1_POOF1);
      sendCancel(SOCKET_ADDR_ANY, POOFER1_POOF1);
    }

    if (touch_sensor.changed(POOFER2_QUICK_POOF_SENSOR) &&
        touch_sensor.touched(POOFER2_QUICK_POOF_SENSOR)) {
      sendBurst(SOCKET_ADDR_ANY, POOFER1_POOF2, 50);
    }

    if (touch_sensor.touched(POOFER2_LONG_POOF_SENSOR)) {
      sendBurst(SOCKET_ADDR_ANY, POOFER1_POOF2, 250);
    } else if (touch_sensor.changed(POOFER2_LONG_POOF_SENSOR)) {
      sendOff(SOCKET_ADDR_ANY, POOFER1_POOF2);
      sendCancel(SOCKET_ADDR_ANY, POOFER1_POOF2);
    }
  }

  /* Change display mode */
  if (touch_sensor.changed(SENSOR_LCD_LEFT) &&
      touch_sensor.touched(SENSOR_LCD_LEFT)) {
    lcd.clear();
    display_mode = (display_mode + 1) % NUM_DISPLAY_MODES;
  }
}

void initialize_display() {
  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("Initializing");
  lcd.setBacklight(HIGH);
}


void update_lcd() {
  uint32_t now = millis();
  static uint32_t last_update = 0;

  switch (display_mode) {
    case 1: {
      if (data_changed) {
        lcd.setCursor(0, 0);
        lcd.print("C:");
        for (byte i = 0; i < MPR121::MAX_SENSORS; i++) {
          lcd.print(touch_sensor.touched(i));
        }
        lcd.print("    ");

        lcd.setCursor(0, 1);
        lcd.print("S:");
        for (byte i = 0; i < NUM_SWITCHES; i++) {
          lcd.print(switch_states[i]);
        }
        lcd.print("      ");
    
        data_changed = false;
      }
      break;
    }
    case 0: {
#if 0
      boolean changed1 = poof1.checkChanged();
      boolean changed2 = poof2.checkChanged();
      boolean updated = changed1 || changed2
        || ((now - last_update) > 1000);

      if (updated) {
        last_update = now;

        lcd.setCursor(0, 0);
        lcd.print("FIRE 1:");
        if (poof1.igniterOn()) {
          lcd.print(" I");
          uint32_t remaining = poof1.ignite_remaining() / 1000;
          lcd.print(remaining);
        } else if (poof1.igniterEnabled()) {
          lcd.print(" E");
        } else {
          lcd.print(" -");
        }

        if (poof1.poofOn()) {
          lcd.print(" P");
        } else if (poof1.poofEnabled() && poof1.poofReady()) {
          lcd.print(" R");
        } else if (poof1.poofEnabled()) {
          lcd.print(" E");
        } else {
          lcd.print(" -");
        }
        lcd.print("     ");

        lcd.setCursor(0, 1);
        lcd.print("FIRE 2:");
        if (poof2.igniterOn()) {
          lcd.print(" I");\
          uint32_t remaining = poof2.ignite_remaining() / 1000;
          lcd.print(remaining);
        } else if (poof2.igniterEnabled()) {
          lcd.print(" E");
        } else {
          lcd.print(" -");
        }

        if (poof2.poofOn()) {
          lcd.print(" P");
        } else if (poof2.poofEnabled() && poof2.poofReady()) {
          lcd.print(" R");
        } else if (poof2.poofEnabled()) {
          lcd.print(" E");
        } else {
          lcd.print(" -");
        }
        lcd.print("     ");

        /*
         * Update the bottom LEDs based on current state
         */
        for (byte i = 8; i < 14; i++) {
          if (poof1.igniterOn()) {
            pixels.setPixelRGB(i % 12, 128, 0, 0);
          } else if (poof1.poofEnabled() && poof1.poofReady()) {
            pixels.setPixelRGB(i % 12, 0, 255, 0);
          } else if (poof1.igniterEnabled()) {
            pixels.setPixelRGB(i % 12, 128, 128, 0);
          } else {
            pixels.setPixelRGB(i % 12, 255, 0, 255);
          }
        }

        for (byte i = 2; i < 8; i++) {
          if (poof2.igniterOn()) {
            pixels.setPixelRGB(i % 12, 128, 0, 0);
          } else if (poof2.poofEnabled() && poof2.poofReady()) {
            pixels.setPixelRGB(i % 12, 0, 255, 0);
          } else if (poof2.igniterEnabled()) {
            pixels.setPixelRGB(i % 12, 128, 128, 0);
          } else {
            pixels.setPixelRGB(i % 12, 255, 0, 255);
          }
        }

        pixels.update();
      }
#endif

      break;
    }
  }
}

void update_poofers() {
}
