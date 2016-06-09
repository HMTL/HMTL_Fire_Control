/*******************************************************************************
 * Author: Adam Phelps
 * License: Create Commons Attribution-Non-Commercial
 * Copyright: 2016
 *
 * Message handling and pendant modes
 ******************************************************************************/

#ifndef WIRELESSPENDANT_MODES_H
#define WIRELESSPENDANT_MODES_H

#include <HMTLPrograms.h>

/* Initialize the message and mode handlers */
void init_modes(Socket **sockets, byte num_sockets);

/* Check for messages and handle program modes */
boolean messages_and_modes(void);

/*******************************************************************************
 * Pendant-specific program modes
 */

/* Wireless pendant programs */
#define PENDANT_TEST_PIXELS 0x20

/* Mode program prototypes */
boolean program_test_pixels_init(msg_program_t *msg,
                                 program_tracker_t *tracker,
                                 output_hdr_t *output);

boolean program_test_pixels(output_hdr_t *output, void *object,
                            program_tracker_t *tracker);

#endif //WIRELESSPENDANT_MODES_H
