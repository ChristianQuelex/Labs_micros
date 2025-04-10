/*
 * Lab4.c
 *
 * Created: 4/04/2025
 * Author : Christian Quelex
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

uint8_t counter = 0;
uint8_t display[16] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};

void initADC(void);
void initPCINT0(void);

int main(void)
{
	cli();				// deactivate interruption 
	DDRC |= (1 << PINC1) | (1 << PINC2) | (1 << PINC3) | (1 << PINC4);		// output 
	PORTC = 0;
	PORTC |= (1 << PORTC1);			// one active 
	
	DDRD = 0xFF;				// output 
	PORTD = 0;
	UCSR0B = 0;					// off us art 
	
	PORTB |= (1 << PINB2) | (1 << PINB3);		//pins as output with pull up
	DDRB &= ~(1 << PINB2) | ~(1 << PINB3);		//inputs
	
	
	
	initADC();					// configuration ADC
	initPCINT0();				// configuration Buttons
	sei();						//active interruption 

	//infinite loop
    while (1) 
    {
		ADCSRA |= (1 << ADSC);				// star conversion ADC
		_delay_ms(5);
		
		uint8_t decValue = ADCH;			//show in portd value of adc
		uint8_t value1 = decValue / 16;		//cociente  more significant
		uint8_t value2 = decValue % 16;		//resto		less significant  
		
		//mux
		PORTC = 0;
		PORTC |= (1 << PORTC1);
		PORTC &= ~(1 << PORTC2) | ~(1 << PORTC3);
		PORTD = counter;					// show leds
		_delay_ms(5);
		
		PORTC = 0;
		PORTC |= (1 << PORTC2);
		PORTC &= ~(1 << PORTC1) | ~(1 << PORTC3);
		PORTD = display[value1];
		_delay_ms(5);						// show pc2
		
		PORTC = 0;
		PORTC |= (1 << PORTC3);
		PORTC &= ~(1 << PORTC2) | ~(1 << PORTC1);
		PORTD = display[value2];			// show pc3
		_delay_ms(5); 
		
		if (decValue > counter){
			PORTC |= (1 << PORTC4);
		}else{
			PORTC &= ~(1 << PORTC4);
		}
    }
}

void initADC(void){
	ADMUX = 0;
	//Vref = AVcc = 5Vs
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
	
	ADMUX |= (1 << ADLAR);	//left adjust
	
	ADCSRA = 0;
	ADCSRA |= (1 << ADEN);	//turn on ADC
	ADCSRA |= (1 << ADIE);	//interruption
	
	//prescaler 128 > 125kHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	DIDR0 |= (1 << ADC0D);	//disable PC0 digital input 
}

ISR (ADC_vect){
	//PORTD = ADCH;			//show in portd value of adc
	ADCSRA |= (1 << ADIF);	//turn off flag
}

void initPCINT0(void){
	PCICR |= (1 << PCIE0);			//interruption pin change
	PCMSK0 |= (1 << PCINT2) | (1 << PCINT3);	//mask
}

ISR (PCINT0_vect){
	if (!(PINB & (1 << PINB2))) {
		counter++;					// increment 
	}else if (!(PINB & (1 << PINB3))) {
		counter--;					// decrement 
	}
}