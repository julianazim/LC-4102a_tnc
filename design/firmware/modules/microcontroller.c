/* --------------------------------
 * LibreCube LC41-01a UHF FSK Radio
 * --------------------------------
 */

#include "microcontroller.h"

void Reset_Sources_Init()
{
    uint16_t i = 0;
    VDM0CN = 0x80;
    for (i = 0; i < 350; i++)
        ;    // Wait 100us for initialization
}

void PCA_Init() /* disable watch dog */
{
    PCA0MD &= ~0x40;
    PCA0MD = 0x00;
}

void Timer_Init()
{
    TCON = 0x50;
    TMOD = 0x21;
    CKCON = 0x09;
    TH1 = 0x96;
}

void UART_Init()
{
    SCON0 = 0x10;
}

void Comparator_Init()
{
    uint16_t i = 0;
    CPT0CN = 0x8F;
    for (i = 0; i < 35; i++)
        ;    // Wait 10us for initialization
    CPT0CN &= ~0x30;
    CPT0MD = 0x03;
}

void Voltage_Reference_Init()
{
    REF0CN = 0x0E;
}

void Port_IO_Init()
{
    // P0.0  -  CP0 (Cmp0),  Push-Pull,  Digital
    // P0.1  -  Unassigned,  Open-Drain, Digital
    // P0.2  -  Unassigned,  Open-Drain, Digital
    // P0.3  -  Unassigned,  Open-Drain, Digital
    // P0.4  -  TX0 (UART0), Push-Pull,  Digital
    // P0.5  -  RX0 (UART0), Open-Drain, Digital
    // P0.6  -  Unassigned,  Open-Drain, Digital
    // P0.7  -  Unassigned,  Open-Drain, Digital
    
    // P1.0  -  Skipped,     Open-Drain, Analog
    // P1.1  -  Skipped,     Open-Drain, Analog
    // P1.2  -  Unassigned,  Open-Drain, Digital
    // P1.3  -  Unassigned,  Open-Drain, Digital
    // P1.4  -  Unassigned,  Open-Drain, Digital
    // P1.5  -  Unassigned,  Open-Drain, Digital
    // P1.6  -  Unassigned,  Open-Drain, Digital
    // P1.7  -  Unassigned,  Push-Pull,  Digital
    // P2.0  -  Unassigned,  Push-Pull,  Digital
    // P2.1  -  Unassigned,  Push-Pull,  Digital
    // P2.2  -  Unassigned,  Push-Pull,  Digital
    // P2.3  -  Unassigned,  Push-Pull,  Digital
    
    P1MDIN = 0xFC;
    P0MDOUT = 0x11;
    P1MDOUT = 0x80;
    P2MDOUT = 0xFF;
    P3MDOUT = 0x1F;
    P1SKIP = 0x03;
    XBR0 = 0x11;
    XBR1 = 0x40;
}

void Oscillator_Init()
{
    OSCICN = 0x83;
}

void Interrupts_Init()
{
    IE = 0x90;
}

void MicrocontrollerInit(void)
{
    PCA_Init();
    Reset_Sources_Init();
    Timer_Init();
    UART_Init();					// 115200 bps
    Comparator_Init();
    Voltage_Reference_Init();
    Port_IO_Init();
    Oscillator_Init();
    Interrupts_Init();
}

void TimerWait(const uint16_t duration)
{
    uint16_t i;
    
    TL0 = 0;
    TH0 = 0;
    do
    {
        i = (uint16_t) (TH0 << 8) + TL0;
    }
    while (i < duration);
}

void TimerReset(void)
{
    TH0 = 0;
    TL0 = 0;
}

uint8_t TimerElapsed(const uint16_t duration)
{
    uint16_t i;
    
    i = (uint16_t) (TH0 << 8) + TL0;
    
    if (i >= duration)
        return PASS;
    else
        return FAIL;
}
