/*
 * PWM2.c
 *
 * Created: 11/04/2025 13:26:14
 *  Author: Chris Q
 */ 
// PWM2.c
#include "PWM2.h"

void initFastPWM2(uint8_t inverted, uint16_t prescaler){
	// Reset Timer2 control registers
	TCCR2A = 0;
	TCCR2B = 0;

	// Prescaler
	switch (prescaler) {
		case 64:
		TCCR2B |= (1 << CS22); // clk/64
		break;
		case 128:
		TCCR2B |= (1 << CS22) | (1 << CS20); // clk/128
		break;
		case 256:
		TCCR2B |= (1 << CS22) | (1 << CS21); // clk/256
		break;
		case 1024:
		TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // clk/1024
		break;
		default:
		TCCR2B |= (1 << CS22); // default to 64
	}

	// Fast PWM mode (WGM2[2:0] = 011)
	TCCR2A |= (1 << WGM20) | (1 << WGM21);

	// No forced output compare
	// Output pin config set in `channel2`
}

void channel2(uint8_t setChannel, uint8_t inverted){
	switch(setChannel){
		case channelA:
		if(inverted){
			TCCR2A |= (1 << COM2A0) | (1 << COM2A1); // Inverted on OC2A (PB3)
			} else {
			TCCR2A |= (1 << COM2A1); // Non-inverted on OC2A
		}
		DDRB |= (1 << PB3); // PB3 = OC2A = output
		break;

		case channelB:
		if(inverted){
			TCCR2A |= (1 << COM2B0) | (1 << COM2B1); // Inverted on OC2B (PD3)
			} else {
			TCCR2A |= (1 << COM2B1); // Non-inverted on OC2B
		}
		DDRD |= (1 << PD3); // PD3 = OC2B = output
		break;
	}
}

void convertServo2(uint16_t analogIn, uint8_t selChannel){
	uint8_t j;
	j = (uint8_t)(((200.0 / 1024.0) * analogIn) + 25); // Ajuste para servo (duty 5% a 10%)
	
	switch(selChannel){
		case channelA:
		OCR2A = j;
		break;
		case channelB:
		OCR2B = j;
		break;
	}
}