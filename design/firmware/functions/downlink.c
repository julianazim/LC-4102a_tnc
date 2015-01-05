/* --------------------------
 * LibreCube LC-4102a 1k2 TNC
 * --------------------------
 */

#include "downlink.h"

volatile uint8_t invertFlag = CLEAR;

uint8_t DetectCaduAsm(void)
{
    static uint32_t asmBuffer = 0;
    uint8_t symbol;
    
    symbol = COMP0;
    
    TimerReset(); /* reset timer */
    while ((COMP0 == symbol) && (TimerElapsed(FULL_SYMBOL) == FAIL))
        ;    /* wait for either signal change or timeout */
        
    if (TimerElapsed(FULL_SYMBOL) == FAIL)    /* signal change occurred */
    {
        TimerWait(HALF_SYMBOL);
        if (symbol)
            symbol = 0;
        else
            symbol = 1;
    }
    asmBuffer = (asmBuffer << 1);
    asmBuffer |= (uint32_t) (symbol & 0x01);
    
    if (asmBuffer == ASM)
    {
        invertFlag = CLEAR;
        return PASS;
    }
    else if (asmBuffer == _ASM)
    {
        invertFlag = SET;
        return PASS;
    }
    else
    {
        return FAIL;
    }
}

/* ReceiveCadu:
 * Receive a CADU from radio and send byte-wise to PC over UART using KISS protocol
 */
void ReceiveCadu(const uint16_t caduLength)
{
    uint16_t i, j;
    uint8_t buffer, symbol;
    
    EA = 0;    // disable all interrupts
    symbol = COMP0;
    
    /* Start the redirect of received octets to the computer */
    TI0 = 0;
    SBUF0 = FEND;
    while (!TI0)
        ;
    
    TI0 = 0;
    SBUF0 = DOWNLINK_STREAM;
    while (!TI0)
        ;
     
    TI0 = 0;
    SBUF0 = (ASM >> 24) & 0xFF;
    while (!TI0)
        ;
    
    TI0 = 0;
    SBUF0 = (ASM >> 16) & 0xFF;
    while (!TI0)
        ;
    
    TI0 = 0;
    SBUF0 = (ASM >> 8) & 0xFF;
    while (!TI0)
        ;
    
    TI0 = 0;
    SBUF0 = ASM & 0xFF;
    while (!TI0)
        ;
    
    for (i = 0; i < (caduLength - ASM_OCTETS); i++)
    {
        buffer = 0;
        for (j = 0; j < 8; j++)
        {
            TimerReset();
            while ((COMP0 == symbol) && (TimerElapsed(FULL_SYMBOL) == FAIL))
                ;
            
            if (TimerElapsed(FULL_SYMBOL) == FAIL)    // transition occurred
            {
                TimerWait(HALF_SYMBOL);
                if (symbol)
                    symbol = 0;
                else
                    symbol = 1;
            }
            
            buffer |= ((uint32_t) (symbol & 0x01)) << (7 - j);
        }
        if (invertFlag)
            buffer = ~buffer;
        
        if (buffer == FEND)    // transpose if FEND octet appears
        {
            TI0 = 0;
            SBUF0 = FESC;
            while (!TI0)
                ;
            TI0 = 0;
            SBUF0 = TFEND;
            while (!TI0)
                ;
        }
        else if (buffer == FESC)    // transpose if FESC octet appears
        {
            TI0 = 0;
            SBUF0 = FESC;
            while (!TI0)
                ;
            TI0 = 0;
            SBUF0 = TFESC;
            while (!TI0)
                ;
        }
        else    // send octet
        {
            TI0 = 0;
            SBUF0 = buffer;
            while (!TI0)
                ;
        }
    }
    
    TI0 = 0;
    SBUF0 = FEND;
    while (!TI0)
        ;
    EA = 1;    // enable interrupts
}
