/* --------------------------
 * LibreCube LC-4102a 1k2 TNC
 * --------------------------
 */

#ifndef TELEMETRY_H
#define TELEMETRY_H

/* Includes */
#include "../definitions.h"
#include "../modules/c8051f310.h"
#include "../modules/microcontroller.h"

/* CCSDS frame protocol specific */
#define	ASM         0x1ACFFC1DUL	// CCSDS attached sync marker: 0001 1010 1100 1111 1111 1100 0001 1101		
#define	_ASM        0xE53003E2UL	// CCSDS attached sync marker inverted: 1110 0101 0011 0000 0000 0011 1110 0010	
#define ASM_OCTETS  4

/* Function declarations */
uint8_t DetectCaduAsm(void);
void ReceiveCadu(const uint16_t cadu_length);

#endif
