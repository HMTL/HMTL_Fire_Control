/*******************************************************************************
 * Author: Adam Phelps
 * License: Create Commons Attribution-Non-Commercial
 * Copyright: 2016
 *
 * Message handling
 ******************************************************************************/

#ifdef DEBUG_LEVEL_MODES
#define DEBUG_LEVEL DEBUG_LEVEL_MODES
#endif
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_HIGH
#endif
#include "Debug.h"

#include <Arduino.h>

#include <ProgramManager.h>
#include <HMTLMessaging.h>
#include <HMTLPrograms.h>

#include <TimeSync.h>
#include <MessageHandler.h>

#include <PixelUtil.h>
#include <HMTLTypes.h>
#include <GeneralUtils.h>

#include "HMTL_Fire_Control.h"
#include "modes.h"
#include "Fire_Control_Sensors.h"

/* List of available programs */
hmtl_program_t program_functions[] = {
        // Programs from HMTLPrograms
        { HMTL_PROGRAM_NONE, NULL, NULL},
        { HMTL_PROGRAM_BLINK, program_blink, program_blink_init },
        //{ HMTL_PROGRAM_TIMED_CHANGE, program_timed_change, program_timed_change_init },
        //{ HMTL_PROGRAM_FADE, program_fade, program_fade_init }
        { HMTL_PROGRAM_SPARKLE, program_sparkle, program_sparkle_init },
        { HMTL_PROGRAM_CIRCULAR, program_circular, program_circular_init}

        // Custom programs
};
#define NUM_PROGRAMS (sizeof (program_functions) / sizeof (hmtl_program_t))

program_tracker_t *active_programs[HMTL_MAX_OUTPUTS];
ProgramManager manager;
MessageHandler handler;

/* Return the first output of the indicated type */
uint8_t find_output_type(uint8_t type) {
  for (byte i = 0; i < config.num_outputs; i++) {
    if (outputs[i]->type == type) {
      return i;
    }
  }
  return HMTL_NO_OUTPUT;
}

/* Construct a sparkle command and run it locally */
void setSparkle() {
  program_sparkle_fmt(rs485.send_buffer, rs485.send_data_size,
                      config.address, find_output_type(HMTL_OUTPUT_PIXELS),
                      100, 0, 0, 0, 0, 0, 0, 0, 0, 0);
  handler.process_msg((msg_hdr_t *)rs485.send_buffer, &rs485, NULL, &config);
}

void setBlink() {
  hmtl_program_blink_fmt(rs485.send_buffer, rs485.send_data_size,
                         config.address, find_output_type(HMTL_OUTPUT_PIXELS),
                         500, pixel_color(255,0,0),
                         250, 0);
  handler.process_msg((msg_hdr_t *)rs485.send_buffer, &rs485, NULL, &config);
}

void setCancel() {
  hmtl_program_cancel_fmt(rs485.send_buffer, rs485.send_data_size,
                          config.address, find_output_type(HMTL_OUTPUT_PIXELS));
  handler.process_msg((msg_hdr_t *)rs485.send_buffer, &rs485, NULL, &config);
}

/*
 * Execute initial commands
 */
void startup_commands() {
  setSparkle();
}

void init_modes(Socket **sockets, byte num_sockets) {
  /* Setup the program manager */
  manager = ProgramManager(outputs, active_programs, objects, HMTL_MAX_OUTPUTS,
                           program_functions, NUM_PROGRAMS);

  /* Setup a message handler with the program manager */
  handler = MessageHandler(config.address, &manager, sockets, num_sockets);

  /* Execute any initial commands */
  startup_commands();
}

/*
 * Check for and handle incoming messages
 */
boolean messages_and_modes(void) {
  // Check and send a serial-ready message if needed
  handler.serial_ready();

  /*
   * Check the serial device and all sockets for messages, forwarding them and
   * processing them if they are for this module.
   */
  boolean update = handler.check(&config);

  /* Execute any active programs */
  if (manager.run()) {
    update = true;
  }

  /* Execute any follow-up changes */
  if (followup_actions()) {
    update = true;
  }

  if (update) {
    /*
     * An output may have been updated by message or active program,
     * update all output states.
     */
    for (byte i = 0; i < config.num_outputs; i++) {
      hmtl_update_output(outputs[i], objects[i]);
    }
  }

  return update;
}

/*
 * Perform an action after all programs have been executed.  This can be used
 * for things like overriding LED values and things of that nature.
 */
boolean followup_actions() {
  boolean changed = false;

  for (byte i = 0; i < 12; i++) {
    if (touch_sensor.touched(i)) {
      pixels.setPixelRGB(sensor_to_led(i), 255,0,0);
      if (touch_sensor.changed(i)) {
        /* Always set the value in case another method had changed it,
         * but we only need to force an update if this was a change
         */
        changed = true;
      }

    } else if (touch_sensor.changed(i)) {
      pixels.setPixelRGB(sensor_to_led(i), 0,0,0);
      changed = true;
    }

  }

  return changed;
}