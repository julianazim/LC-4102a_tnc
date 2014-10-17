/* --------------------------------
 * LibreCube LC41-01a UHF FSK Radio
 * --------------------------------
 */

#ifndef MICROCONTROLLER_H
#define MICROCONTROLLER_H

/* Includes */
#include "../definitions.h"
#include "c8051f310.h"

/* Pin allocations */
#define COMP0			P0_0
#define COMP0_POS		P1_0
#define COMP0_NEG		P1_1
#define DATAOUT			P1_7
#define _PTT			P2
#define LED_POWER		P3_1
#define LED_TX			P3_2
#define LED_RX			P3_3

/* Port settings */
#define PTT_OFF         0xFF    // set entire port to high
#define PTT_ON          0x00    // set entire port to low

/* Function declarations */ 
void MicrocontrollerInit(void);
void TimerWait(const uint16_t duration);
void TimerReset(void);
uint8_t TimerElapsed(const uint16_t duration);

#endif
