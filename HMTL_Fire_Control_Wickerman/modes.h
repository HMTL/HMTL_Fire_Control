/*******************************************************************************
 * Author: Adam Phelps
 * License: Create Commons Attribution-Non-Commercial
 * Copyright: 2016
 *
 * Message handling and output modes
 ******************************************************************************/

#ifndef MODES_H
#define MODES_H

#include <HMTLPrograms.h>

/* Initialize the message and mode handlers */
void init_modes(Socket **sockets, byte num_sockets);

/* Check for messages and handle program modes */
boolean messages_and_modes(void);

void setSparkle();
void setBlink(uint32_t color);
void setCancel();


boolean followup_actions();

/*******************************************************************************
 * Control box specific modes
 */

#endif //MODES_H
