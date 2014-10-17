/* --------------------------
 * LibreCube LC-4102a 1k2 TNC
 * --------------------------
 */

#ifndef UPLINK_H
#define UPLINK_H

/* Includes */
#include "../definitions.h"
#include "../modules/c8051f310.h"
#include "../modules/microcontroller.h"

/* CCSDS frame protocol specific */
#define IDLE_OCTET      0x55

/* Function declarations */
void SendCltu(const uint8_t* cltu, const uint16_t len);
void SendIdle(void);

#endif
