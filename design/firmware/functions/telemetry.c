/* --------------------------
 * LibreCube LC-4102a 1k2 TNC
 * --------------------------
 */

#include "telemetry.h"

volatile uint8_t invert_flag = CLEAR;

uint8_t DetectCaduAsm(void)
{
	static uint32_t asm_buffer = 0;
    uint8_t symbol;
	
	symbol = COMP0;
 
	TimerReset(); // reset timer
	while ((COMP0 == symbol) && (TimerElapsed(FULL_SYMBOL) == FAIL)); // wait for signal change or timeout

	if (TimerElapsed(FULL_SYMBOL) == FAIL) // signal change occured
	{
		TimerWait(HALF_SYMBOL);
		if (symbol) symbol = 0; else symbol = 1;
	}
	asm_buffer = asm_buffer << 1;
	asm_buffer |= (unsigned long)(symbol & 0x01);

	if (asm_buffer == ASM)
	{
		invert_flag = CLEAR;
		return PASS;
	}
	else if (asm_buffer == _ASM)
	{
		invert_flag = SET;
		return PASS;
	}
	else
	{
		return FAIL;
	}
}

void ReceiveCadu(uint16_t cadu_length) // receive CADU from radio and send octetwise to PC over UART KISS protocol
{
	uint16_t i, j;
	uint8_t buffer, symbol;

	EA = 0; // disable all interrupts
	symbol = COMP0;
 
	TI0 = 0;		
	SBUF0 = FEND; 
	while(!TI0);

	TI0 = 0;		
	SBUF0 = DOWNLINK_STREAM; 
	while(!TI0);

	TI0 = 0;		
	SBUF0 = (ASM >> 24) & 0xFF; 
	while(!TI0);

	TI0 = 0;		
	SBUF0 = (ASM >> 16) & 0xFF; 
	while(!TI0);

	TI0 = 0;		
	SBUF0 = (ASM >> 8) & 0xFF; 
	while(!TI0);

	TI0 = 0;		
	SBUF0 = ASM & 0xFF; 
	while(!TI0);

	for (i = 0; i < cadu_length - ASM_OCTETS; i++)
	{
		buffer = 0;
		for (j = 0; j < 8; j++)
		{
			TimerReset();
			while ((COMP0 == symbol) && (TimerElapsed(FULL_SYMBOL) == FAIL));

			if (TimerElapsed(FULL_SYMBOL) == FAIL) // transition occured
			{
				TimerWait(HALF_SYMBOL);
				if (symbol) symbol = 0; else symbol = 1;
			}
			
			buffer |= (unsigned long)(symbol & 0x01) << (7-j);
		}
		if (invert_flag) buffer = ~buffer;

		if (buffer == FEND) // transpose if FEND octet appears
		{
			TI0 = 0;		
			SBUF0 = FESC; 
			while(!TI0);
			TI0 = 0;		
			SBUF0 = TFEND; 
			while(!TI0);
		}
		else if (buffer == FESC) // transpose if FESC octet appears
		{
			TI0 = 0;		
			SBUF0 = FESC; 
			while(!TI0);
			TI0 = 0;		
			SBUF0 = TFESC; 
			while(!TI0);
		}
		else // send octet
		{
			TI0 = 0;		
			SBUF0 = buffer; 
			while(!TI0);
		}
	}

	TI0 = 0;		
	SBUF0 = FEND; 
	while(!TI0);
	EA = 1; // enable interrupts
}
