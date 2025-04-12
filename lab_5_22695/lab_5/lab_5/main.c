/*
 * Prelab5.c
 *
 * Created: 11/04/2025 01:13:57 a. m.
 * Author : Christian Quelex
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "PWM1/PWM1.h"
#include "PWM2/PWM2.h"


void initADC(void);

uint16_t readADC(uint8_t canal);


int main(void)
{
	cli();
	DDRB |= (1 << PORTB2);		//PB2 as output (OC0A and OCR0B)
	DDRD |= (1 << PORTD3);		//pD3 control B
	/*TCCR1A = 0;
	TCCR1B = 0;*/
	
	DDRC = 0;		//Puerto C como entrada 
	
	
	initFastPWM1(settedUp, 8);		//pwm1 precaler 8
	channel(channelB, nop);			//salida no invertida 
	topValue(39999);				// top para 50hz
	
	initFastPWM2(nop, 256);			//timer 2, prescaler 64
	channel2(channelB, nop);		//canal B PD3
	
	
	/*
	//no inverted and fast PWM, with ICR1 as TOP
	TCCR1A |= (1 << COM1B1);
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12) | (1 << WGM13);
	
	//Prescaler 8
	TCCR1B |= (1 << CS11);
	
	//setting TOP value, 39999
	ICR1 = 39999;
	*/
	
	/*
	uint16_t i = 0;
	//uint8_t restart = 0;
	uint16_t j = 1000;
	OCR1B = j;
	*/
	initADC();  // Activa el ADC (con ADCH como salida de 8 bits)
	sei();
	
	while (1)
	{
		 uint16_t valorPC0 = readADC(0);
		 convertServo(valorPC0, channelB);

		 _delay_ms(10);

		 // Leer PC1 (ADC1) y controlar servo 2 (Timer2)
		 uint16_t valorPC1 = readADC(1);
		 convertServo2(valorPC1, channelB);

		 _delay_ms(10);
		
		//ADCSRA |= (1 << ADSC);		// Inicia conversión ADC
		//_delay_ms(2);				// Espera un poco para asegurarse que termina
		
		
		//convertServo(ADCH, channelB);			// Ajusta el duty del canalB según entrada analógica
		
		/*i = ADCH;
		j = (200/12)*i+1000;
		
		OCR1B = j;*/
	}
}




void initADC(void){
	ADMUX = 0;
	//Vref = AVcc = 5Vs
	ADMUX |= (1 << REFS0);		//mas significativos
	ADMUX &= ~(1 << REFS1);		//menos significativos 
	
	ADMUX |= (1 << ADLAR);	//left adjust
	
	ADCSRA = 0;
	ADCSRA |= (1 << ADEN);	//turn on ADC  - Este
	ADCSRA |= (1 << ADIE);	//interruption
	
	//prescaler 128 > 125kHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	DIDR0 |= (1 << ADC0D) | (1 << ADC1D);	//disable PC0 digital input
}

uint16_t readADC(uint8_t canal)
{
	canal &= 0x07;
	
	
	// Configurar el canal
	ADMUX = (ADMUX & 0xF0) | canal;
	
	// Iniciar conversión
	ADCSRA |= (1 << ADSC);
	
	// Esperar a que termine la conversión
	while (ADCSRA & (1 << ADSC));
	
	//ADMUX = (1 << REFS0) | (canal & 0x0F); // Seleccionar canal y referencia
	//_delay_ms(10); // Tiempo de adquisición

	//ADCSRA |= (1 << ADSC); // Iniciar conversión
	//while (ADCSRA & (1 << ADSC)); // Esperar

	return ADCH; // Valor de 10 bits
}



ISR (ADC_vect){
	//PORTD = ADCH;			//show in portd value of adc
	ADCSRA |= (1 << ADIF);	//turn off flag
}
