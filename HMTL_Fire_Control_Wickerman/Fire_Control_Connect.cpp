/*******************************************************************************
 * Author: Adam Phelps
 * License: Create Commons Attribution-Non-Commercial
 * Copyright: 2014
 *
 * Code for communicating with remote modules
 ******************************************************************************/

#include <Arduino.h>
#include "EEPROM.h"
#include <RS485_non_blocking.h>
#include <SoftwareSerial.h>
#include "SPI.h"
#include "Wire.h"
#include "FastLED.h"


#ifdef DEBUG_LEVEL_CONNECT
#define DEBUG_LEVEL DEBUG_LEVEL_CONNECT
#endif
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_HIGH
#endif
#include "Debug.h"

#include "GeneralUtils.h"
#include "EEPromUtils.h"
#include "HMTLTypes.h"
#include <HMTLProtocol.h>
#include "HMTLMessaging.h"
#include "HMTLPrograms.h"

#include "PixelUtil.h"
#include "RS485Utils.h"
#include "XBeeSocket.h"
#include "MPR121.h"

#include "HMTL_Fire_Control.h"


void sendHMTLValue(uint16_t address, uint8_t output, int value) {
  DEBUG3_VALUE("sendValue:", value);
  DEBUG3_VALUE(" a:", address);
  DEBUG3_VALUELN(" o:", output);
  hmtl_send_value(&rs485, rs485.send_buffer, SEND_BUFFER_SIZE,
		  address, output, value);
}

void sendHMTLTimedChange(uint16_t address, uint8_t output,
			 uint32_t change_period,
			 uint32_t start_color,
			 uint32_t stop_color) {
  DEBUG3_VALUE("sendTimed:", change_period);
  DEBUG3_VALUE(" a:", address);
  DEBUG3_VALUELN(" o:", output);

  hmtl_send_timed_change(&rs485, rs485.send_buffer, SEND_BUFFER_SIZE,
			 address, output,
			 change_period,
			 start_color,
			 stop_color);
}

void sendHMTLCancel(uint16_t address, uint8_t output) {
  DEBUG3_VALUE("sendCancel: a:", address);
  DEBUG3_VALUELN(" o:", output);

  hmtl_send_cancel(&rs485, rs485.send_buffer, SEND_BUFFER_SIZE,
                   address, output);
}

void sendHMTLBlink(uint16_t address, uint8_t output,
                   uint16_t onperiod, uint32_t oncolor,
                   uint16_t offperiod, uint32_t offcolor) {
  DEBUG3_VALUE("sendBlink:", onperiod);
  DEBUG3_VALUE(",", offperiod);
  DEBUG3_VALUE(" a:", address);
  DEBUG3_VALUELN(" o:", output);

  hmtl_send_blink(&rs485, rs485.send_buffer, SEND_BUFFER_SIZE,
                  address, output,
                  onperiod, oncolor,
                  offperiod, offcolor);
}