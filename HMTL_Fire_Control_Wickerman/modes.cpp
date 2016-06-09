/*******************************************************************************
 * Author: Adam Phelps
 * License: Create Commons Attribution-Non-Commercial
 * Copyright: 2016
 *
 * Message handling and pendant modes
 ******************************************************************************/

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

#include "HMTL_Fire_Control.h"
#include "modes.h"

/* List of available programs */
hmtl_program_t program_functions[] = {
        // Programs from HMTLPrograms
        { HMTL_PROGRAM_NONE, NULL, NULL},
        //{ HMTL_PROGRAM_BLINK, program_blink, program_blink_init },
        //{ HMTL_PROGRAM_TIMED_CHANGE, program_timed_change, program_timed_change_init },
        //{ HMTL_PROGRAM_FADE, program_fade, program_fade_init },

        // Custom programs
};
#define NUM_PROGRAMS (sizeof (program_functions) / sizeof (hmtl_program_t))

program_tracker_t *active_programs[HMTL_MAX_OUTPUTS];
ProgramManager manager;
MessageHandler handler;

/*
 * Execute initial commands
 */
void startup_commands() {
//  const byte data[] = { // This turns on PENDANT_TEST_PIXELS for all outputs
//          0xfc,0x00,0x02,0x17,0x01,0x00,0xff,0xff,
//          0x03,0xfe,0x20,0x32,0x00,0x00,0x00,0x00,
//          0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//  memcpy(rs485.send_buffer, data, sizeof (data));
//
//  handler.process_msg((msg_hdr_t *)rs485.send_buffer, &rs485,
//                      NULL, &config);

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

  if (update) {
    /*
     * An output may have been updated by message or active program,
     * update all output states.
     */
    for (byte i = 0; i < config.num_outputs; i++) {
      hmtl_update_output(outputs[i], objects[i]);
    }
  }
}

