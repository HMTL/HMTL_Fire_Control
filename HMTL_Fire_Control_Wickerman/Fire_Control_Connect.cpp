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


#define DEBUG_LEVEL DEBUG_MID
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
  hmtl_send_value(&rs485, rs485.send_buffer, SEND_BUFFER_SIZE,
		  address, output, value);
}

void sendHMTLTimedChange(uint16_t address, uint8_t output,
			 uint32_t change_period,
			 uint32_t start_color,
			 uint32_t stop_color) {
  hmtl_send_timed_change(&rs485, rs485.send_buffer, SEND_BUFFER_SIZE,
			 address, output,
			 change_period,
			 start_color,
			 stop_color);
}

void sendHMTLCancel(uint16_t address, uint8_t output) {
  hmtl_send_cancel(&rs485, rs485.send_buffer, SEND_BUFFER_SIZE,
                   address, output);
}