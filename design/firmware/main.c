/* --------------------------
 * LibreCube LC-4102a 1k2 TNC
 * --------------------------
 * Last update: 2014-10-17 
 */

/* 	Summary:
 *		The TNC provides is the node between computer and radio system for sending 
 *		telecommand frames (CLTUs) and receiving telemetry frames (CADUs).
 *		
 *      When not sending, the TNC continuously reads from the radio output, 
 *      shifts these bits into a small buffer, and checks if the synch pattern
 *      (ASM) has been detected. It then receives the CADU (i.e. telemetry) frame
 *      bit for bit, shifts it into an octet buffer (i.e. byte buffer) and sends
 *      appends each completed octet to the frame being send to the computer
 *      via the KISS protocol. Since CADU frames have fixed length, the TNC 
 *      knows when a full CADU has been received and then terminates the 
 *      transfer to the computer.
 *      
 *		For sending of CLTUs (i.e. telecommands) the computer first switches on 
 *		the transmitter, then activates the sending of acquisition sequence 
 *		(which is composed of idle pattern) and then transfers the CLTU via
 *		the KISS protocol to the TNC. Once the whole CLTU has been transferred,
 *		the TNC starts to transmit the CLTU bit stream to the radio, provided
 *		that the transmitter is left enabled. The computer finishes the transmit
 *		session by switching of the transmitter.
 *
 *	Configuration:
 *		- uart baudrate (115200)
 *		- modem baudrate (1k2)
 *		- maximum cltu buffer size (210 octets)
 *		- default cadu length (64 octets)
 *
 *	To do:
 *		- allow option of inverted uplink bit stream?
 */

/* Includes */
#include "definitions.h"
#include "functions/downlink.h"
#include "functions/uplink.h"

/* Globals */
__xdata uint8_t cltuBuffer[CLTU_BUFFER_SIZE]; /* buffers a CLTU from the PC for sending */
uint8_t cltuReceived = CLEAR; /* flags if CLTU was received form computer */
uint8_t idleSequence = CLEAR; /* flags if idle sequence shall be transmitted */
uint8_t transmitterMode = OFF; /* flags if transmitter shall be on */
uint16_t octetCount = 0; /* counts the length of the buffered CLTU */
uint16_t caduLength = DEFAULT_CADU_LENGTH; /* defines the fixed length of CADU telemetry frames */

void main(void)
{
    uint32_t i;
    
    MicrocontrollerInit();
    LED_POWER = ON; /* Power LED on */
    _PTT = PTT_OFF; /* Transmitter (via Push-to-talk) off */
    
	/* Show all LEDs at power-up */
    LED_TX = ON;
    LED_RX = ON;
    for (i = 0; i < 1000000UL; i++); 
    LED_TX = OFF;
    LED_RX = OFF;

    for (;;)
    {       
        if (transmitterMode == ON)
        {
            if (cltuReceived) /* send CLTU if available */
            {
                cltuReceived = CLEAR;
                SendCltu(cltuBuffer, octetCount);
            }
            else if (idleSequence) /* otherwise send idle pattern if enabled */
            {
                SendIdle();
            }         
        }
        else /* TNC is in receiving mode */
        {
            if (DetectCaduAsm() == PASS) /* if the synch marker is detected from radio */
            {
                LED_RX = ON;
                ReceiveCadu(caduLength); /* receive and forward the telemetry */
                LED_RX = OFF;
            }
        }
    }
}

void UartIsr(void) __interrupt 4
{
    static uint8_t uartBuffer = 0;
    
    if (RI0) /* byte received */
    {
        uartBuffer = SBUF0;
        if (uartBuffer == FEND) /* FENDs mark the begin and end of a frame */
        {
            cltuReceived = CLEAR; /* reset all counters */
            octetCount = CLEAR;
            RI0 = 0;
        }
        else /* this byte contains the CMD */
        {
            switch (uartBuffer)
            {
                case TEST_CONNECTION: /* test UART connection */
                    for (;;) /* read remaining bytes of KISS frame */
                    {
                        RI0 = 0;
                        while (!RI0)
                            ;
                        uartBuffer = SBUF0;
                        if (uartBuffer == FEND)
                            break;
                    }
                    
                    TI0 = 0;
                    SBUF0 = FEND; /* send KISS frame start */
                    while (!TI0)
                        ;
                    TI0 = 0;
                    SBUF0 = HANDSHAKE; /* send HANDSHAKE */
                    while (!TI0)
                        ;
                    TI0 = 0;
                    SBUF0 = FEND;
                    while (!TI0)
                        ;
                    break;
                    
                case CADU_LENGTH: /* program fixed CADU length */
                    RI0 = 0;
                    while (!RI0)
                        ;
                    uartBuffer = SBUF0;
                    RI0 = 0;
                    while (!RI0)
                        ;
                    caduLength = uartBuffer + ((uint16_t) SBUF0 << 8);
                    break;
                    
                case TX_STATE: /* set transmitter state */
                    
                    RI0 = 0;
                    while (!RI0)
                        ;
                    uartBuffer = SBUF0;
                    
                    if (uartBuffer == ON) /* set transmitter on */
                    {
                        transmitterMode = ON;
                        _PTT = PTT_ON;
                        LED_TX = ON;
                    }
                    else /* set transmitter off */
                    {
                        transmitterMode = OFF;
                        _PTT = PTT_OFF;
                        LED_TX = OFF;
                    }
                    break;
                    
                case IDLE_SEQUENCE: /* enable or disable sending of idle sequence */
                    
                    RI0 = 0;
                    while (!RI0)
                        ;
                    uartBuffer = SBUF0;
                    
                    if (uartBuffer == ON)
                    {
                        idleSequence = ON;
                    }
                    else if (uartBuffer == OFF)
                    {
                        idleSequence = OFF;
                    }
                    
                    break;
                    
                case UPLINK_CLTU: /* buffer data (CLTU) from computer (to be uplinked) */
                    for (;;)
                    {
                        RI0 = 0;
                        while (!RI0)
                            ;
                        uartBuffer = SBUF0;
                        
                        if (uartBuffer == FEND)
                            break;
                        
                        if (uartBuffer == FESC)
                        {
                            RI0 = 0;
                            while (!RI0)
                                ;
                            uartBuffer = SBUF0;
                            
                            if (uartBuffer == TFESC)
                                uartBuffer = FESC;
                            else if (uartBuffer == TFEND)
                                uartBuffer = FEND;
                        }
                        
                        cltuBuffer[octetCount] = uartBuffer;
                        if (octetCount < (CLTU_BUFFER_SIZE - 1))
                            octetCount++;
                    }
                    cltuReceived = SET; /* frame with data received */
                    break;
            }
            
            RI0 = 0;
        }
    }
    
    if (TI0)    // byte sent
    {
        TI0 = 0;
    }
}
