/*
 * PWM1.c
 *
 * Created: 11/04/2025 10:15:04 a. m.
 *  Author: Christian Quelex 
 */ 

#include "PWM1.h"

uint16_t topVal = 0;		// establecer valor top o no 
uint16_t i, j;		//transition variables

//(channel <- channelA/channelB, inverted <- yes/nop)
void channel(uint8_t setChannel, uint8_t inverted){
	switch(setChannel){
		case 1:		// canal A
			if(inverted == 1){
				TCCR1A |= (1 << COM1A0) | (1 << COM1A1);	//channel A inverted
			}else{
				TCCR1A |= (1 << COM1A1);					//channel A no inverted
			}
		break;
		
		case 2:		// Canal B
			if(inverted == 1){
				TCCR1A |= (1 << COM1B0) | (1 << COM1B1);	//channelB inverted
			}else{
				TCCR1A |= (1 << COM1B1);					//channelB no inverted
			}
		break;
	}
}

/*Settings for Fast PWM 1, 16 bits, use it just one time
(inverted <- yes/nop, modePWM <- normal/settedUp, prescaler <- 1,8,64,256,1024)
*/
void initFastPWM1(uint8_t modePWM, uint16_t prescaler){
	//initialize register timer1
	TCCR1A = 0;			//Clean register 
	TCCR1B = 0;
	
	switch (prescaler)
	{
		case 8:
			TCCR1B |= (1 << CS11);					//prescaler 8
		break;
		
		case 1024:
			TCCR1B |= (1 << CS10) | (1 << CS12);	//prescaler 1024
		break;
	}
	
	//selecting mode of operation
	switch (modePWM)
	{
		case normal:		//fast pwm de 10 bits 
			//fast PWM 10-bit resolution
			TCCR1A |= (1 << WGM11) | (1 << WGM10);
			TCCR1B |= (1 << WGM12);
		break;
		
		case settedUp:		//fast pwm con icr1 como top 
			//fast PWM with ICR1 as TOP
			TCCR1A |= (1 << WGM11);
			TCCR1B |= (1 << WGM12) | (1 << WGM13);
			topVal = 1;		// bandera que permite usar icr1
		break;
	}
	
}

//topValue, used if mode is settedUp
void topValue(uint16_t top){
	if (topVal == 1)	{
		ICR1 = top;		//set top value
	}else{
		topVal = 0;		// ICR1 define el valor maximo del contador 
	}
}

//conversion for servos, mapping values
void convertServo(uint16_t analogIn, uint8_t selChannel){
	// ADCH entrada analogica 8 bits
	// (200/12)
	switch(selChannel){
		case 1:
			i = ADCH;
			j = (200/12)*i+1000; // convierte un rango de 0-255 a 1000-2000
			OCR1A = j;
		break;
		
		case 2:
			i = ADCH;
			j = (200/12)*i+1000;
			OCR1B = j;
		break;
	}
}