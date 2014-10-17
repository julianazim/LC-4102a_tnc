/* --------------------------
 * LibreCube LC-4102a 1k2 TNC
 * --------------------------
 */

#include "uplink.h"

void SendCltu(const uint8_t* cltu, const uint16_t len)
{
    uint16_t i, j;
    uint8_t octet;
    uint8_t octetBit;
    
    for (i = 0; i < len; i++) /* go through all bytes */
    {
        octet = cltu[i];
        
        for (j = 0; j < 8; j++) /* go through the bits of the byte */
        {
            TimerReset();
            octetBit = (octet >> (7 - j)) & 0x01;
            DATAOUT = octetBit;
            while (TimerElapsed(FULL_SYMBOL) == FAIL)
                ; /* this provides the timing for 1k2 baud */
        }
    }
    
    EA = 0; /* disable all interrupts (must be repeated) */
    EA = 0;
    
    /* Confirm completed transmission with handshake */
    TI0 = 0;
    SBUF0 = FEND;
    while (!TI0)
        ;
    TI0 = 0;
    SBUF0 = HANDSHAKE;
    while (!TI0)
        ;
    TI0 = 0;
    SBUF0 = FEND;
    while (!TI0)
        ;
    EA = 1; /* enable interrupts */
}

void SendIdle(void)
{
    static uint8_t idleByte = IDLE_OCTET;
    static uint8_t idleBit;
    static uint8_t i = 0;
    
    TimerReset();
    idleBit = (idleByte >> (7 - i)) & 0x01;
    DATAOUT = idleBit;
    while (TimerElapsed(FULL_SYMBOL) == FAIL)
        ;
    i++;
    if (i >= 8)
        i = 0;
}
