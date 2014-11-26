/* --------------------------
 * LibreCube LC-4102a 1k2 TNC
 * --------------------------
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/* Type definitions */
typedef signed char int8_t;
typedef short int int16_t;
typedef long int int32_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned long int uint32_t;

/* General definitions */
#define PASS                1
#define FAIL                0
#define ON                  1
#define OFF                 0
#define CLEAR               0
#define SET                 1

/* Configuration */
#define CLTU_BUFFER_SIZE 	12//0	
/* In bytes, defines maximum size of CLTU that can be buffered
 * Careful: if chosen too large, firmware will not run (although no
 * error is shown in compilation)!!
 */
#define DEFAULT_CADU_LENGTH		64 
/* default fixed length of a CADU frame including ASM in octets */

/* 1k2 Baud timing formula: 	1/clock * SCANRATE * FULL_SYMBOL = 1/baudrate		
 * with: clock = SYSCLOCK/4 = 6,125 MHz, and baudrate = 1200 bps 
 */ 				
#define SCANRATE		80.0//79.75	 
/* to achieve (1/1200bps)/64 => (833us per bit)/64 = 13us desired scan frequency (64x oversampled)
 * scanrate must be carefully tuned, otherwise no bitlock */
#define QUARTER_SYMBOL	16*SCANRATE
#define HALF_SYMBOL		32*SCANRATE 	/* half the duration of a symbol */
#define FULL_SYMBOL		64*SCANRATE 	/* full duration of a symbol */

/* KISS protocol (TNC to computer) */
#define FEND			    0xC0
#define FESC			    0xDB
#define TFEND			    0xDC
#define TFESC			    0xDD
#define UPLINK_STREAM		0x00
#define DOWNLINK_STREAM		0x02
#define HANDSHAKE			0x0A
#define TEST_CONNECTION		0x0B
#define TRANSMITTER 		0x0C
#define IDLE_SEQUENCE		0x0D
#define CADU_LENGTH			0x0E

#endif
