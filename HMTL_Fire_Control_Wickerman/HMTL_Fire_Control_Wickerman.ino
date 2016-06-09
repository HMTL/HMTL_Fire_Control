/*******************************************************************************
 * Code for the HMTL Fire control module:
 *   - LCD Display
 *   - 4 integral touch sensors, 4 optional external ones
 *   - 4 simple switches
 *
 * Author: Adam Phelps
 * License: Create Commons Attribution-Non-Commercial
 * Copyright: 2016
 ******************************************************************************/

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_HIGH
#endif
#include "Debug.h"

#include "EEPROM.h"
#include <RS485_non_blocking.h>
#include <SoftwareSerial.h>
#include "LiquidCrystal.h"

#include "SPI.h"
#include "FastLED.h"

#include "GeneralUtils.h"
#include "EEPromUtils.h"
#include "HMTLTypes.h"
#include "PixelUtil.h"
#include "Wire.h"
#include "MPR121.h"
#include "SerialCLI.h"

#include "Socket.h"
#include "RS485Utils.h"
#include "XBee.h"
#include "XBeeSocket.h"

#include "HMTLProtocol.h"
#include "HMTLMessaging.h"
#include "HMTLPrograms.h"
#include "TimeSync.h"

#include "HMTL_Fire_Control.h"

#include "modes.h"


/* Auto update build number */
#define HMTL_FIRE_CONTROL_BUILD 7 // %META INCR

//LiquidTWI lcd(0); ??? Why isn't this used?
LiquidCrystal lcd(0);

MPR121 touch_sensor; // MPR121 must be initialized after Wire.begin();
uint16_t my_address = 0;

/******/

config_hdr_t config;
output_hdr_t *outputs[HMTL_MAX_OUTPUTS];
config_max_t readoutputs[HMTL_MAX_OUTPUTS];
void *objects[HMTL_MAX_OUTPUTS];

config_value_t value_output;

PixelUtil pixels;

RS485Socket rs485;
#define SEND_BUFFER_SIZE 64 // The data size for transmission buffers
byte rs485_data_buffer[RS485_BUFFER_TOTAL(SEND_BUFFER_SIZE)];

#define MAX_SOCKETS 2
Socket *sockets[MAX_SOCKETS] = { NULL, NULL };

void setup() {
  Serial.begin(BAUD);
  DEBUG2_PRINTLN("*** HMTL Fire Control Initializing ***");
  DEBUG2_VALUELN("* Baud is ", BAUD);

  /* Initialize random see by reading from an unconnected analog pin */
  randomSeed(analogRead(0) + analogRead(2) + micros());

  /* Initialize display */
  initialize_display();

  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Hello Black");
  lcd.setCursor(0,1);  lcd.print("Rock City!");



  int configOffset = -1;
  int32_t outputs_found = hmtl_setup(&config, readoutputs,
                                     outputs, objects, HMTL_MAX_OUTPUTS,
                                     &rs485,
                                     NULL,          // XBee
                                     &pixels,
                                     &touch_sensor,          // MPR121
                                     NULL,          // RGB
                                     &value_output, // Value
                                     &configOffset);

  DEBUG4_VALUE("Config size:", configOffset - HMTL_CONFIG_ADDR);
  DEBUG4_VALUELN(" end:", configOffset);

  if (!(outputs_found & (1 << HMTL_OUTPUT_RS485))) {
    DEBUG_ERR("No RS485 config found");
    DEBUG_ERR_STATE(1);
  }

  if (!(outputs_found & (1 << HMTL_OUTPUT_PIXELS))) {
    DEBUG_ERR("No pixels config found");
    DEBUG_ERR_STATE(2);
  }

  if (!(outputs_found & (1 << HMTL_OUTPUT_MPR121))) {
    DEBUG_ERR("No mpr121 config found");
    DEBUG_ERR_STATE(3);
  }

  /* Setup communication devices */
  byte num_sockets = 0;

  /* Setup the RS485 connection */
  rs485.setup();
  rs485.initBuffer(rs485_data_buffer, SEND_BUFFER_SIZE);
  sockets[num_sockets++] = &rs485;

  if (num_sockets == 0) {
    DEBUG_ERR("No sockets configured");
    DEBUG_ERR_STATE(2);
  }

  init_modes(sockets, num_sockets);

  /* Setup the sensors */
  initialize_switches();

  DEBUG2_PRINTLN("* Wickerman Fire Control Initialized *");
  DEBUG2_VALUELN(" Build=", HMTL_FIRE_CONTROL_BUILD);
  DEBUG_MEMORY(DEBUG_HIGH);

  pixels.setAllRGB(255, 0, 255);
  pixels.update();

  // Send the ready signal to the serial port
  Serial.println(F(HMTL_READY));
}

#define PERIOD 100
unsigned long last_change = 0;
int cycle = 0;
void loop() {

  // TODO: Add sensors and such here

  /* Check the sensor values */
  sensor_cap();

  sensor_switches();

  handle_sensors();

  // XXX: Do stuff here

  //update_poofers();

  update_lcd();


  /*
   * Check for messages and handle output states
   */
  boolean updated = messages_and_modes();


}
