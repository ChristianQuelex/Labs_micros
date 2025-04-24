/*
 * Prelab5.c - Control de 2 servos + LED con PWM de alta frecuencia
 * Versión completa lista para usar
 */

#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "PWM1/PWM1.h"
#include "PWM2/PWM2.h"

// Configuración LED PWM
#define LED_PIN PB0
volatile uint8_t pwmCounter = 0;
volatile uint8_t pwmValue = 0;

void initADC(void);
uint16_t readADC(uint8_t canal);
void initTimer0_LEDPWM(void);
void updateLEDBrightness(uint8_t brightness);

int main(void)
{
    cli();
    // 1. Configuración de pines
    DDRB |= (1 << PORTB2) | (1 << LED_PIN);  // PB2 (servo1) + LED
    DDRD |= (1 << PORTD3);                   // PD3 (servo2)
    DDRC = 0;                                // Puerto C como entrada
    
    // 2. Inicialización de PWM para servos
    initFastPWM1(settedUp, 8);  // PWM1 con prescaler 8
    channel(channelB, nop);     // Canal B sin invertir
    topValue(39999);            // TOP para 50Hz
    
    initFastPWM2(nop, 256);     // PWM2 con prescaler 256
    channel2(channelB, nop);    // Canal B sin invertir
    
    // 3. Inicialización PWM LED (alta frecuencia)
    initTimer0_LEDPWM();
    
    // 4. Inicialización ADC
    initADC();
    
    sei();

    // Bucle principal
    while (1)
    {
        // Control servo1 (PC0)
        uint16_t valorPC0 = readADC(0);
        convertServo(valorPC0, channelB);

        // Control servo2 (PC1)
        uint16_t valorPC1 = readADC(1);
        convertServo2(valorPC1, channelB);

        // Control LED (PC2) - PWM de alta frecuencia
        updateLEDBrightness(readADC(2) >> 2);  // Convertir 10-bit a 8-bit
        
        _delay_ms(10);
    }
}

// Inicialización PWM LED (7.8kHz)
void initTimer0_LEDPWM(void) {
    TCCR0A = (1 << WGM01);        // Modo CTC (TOP=OCR0A)
    TCCR0B = (1 << CS00);         // Sin prescaler (clock directo)
    OCR0A = 255;                  // Frecuencia máxima (~7.8kHz)
    TIMSK0 = (1 << OCIE0A);       // Habilitar interrupción por comparación
    DDRB |= (1 << LED_PIN);       // Configurar pin como salida
}

// ISR para PWM LED de alta frecuencia
ISR(TIMER0_COMPA_vect) {
    pwmCounter++;
    if(pwmCounter == 0) PORTB |= (1 << LED_PIN);     // Encender
    if(pwmCounter == pwmValue) PORTB &= ~(1 << LED_PIN); // Apagar
}

// Actualizar brillo LED (0-255)
void updateLEDBrightness(uint8_t brightness) {
    pwmValue = brightness;
}

// Función initADC original
void initADC(void){
    ADMUX = 0;
    ADMUX |= (1 << REFS0);      // Referencia AVcc
    ADMUX &= ~(1 << REFS1);     
    ADMUX |= (1 << ADLAR);      // Ajuste a izquierda
    
    ADCSRA = 0;
    ADCSRA |= (1 << ADEN);      // Habilitar ADC
    ADCSRA |= (1 << ADIE);      // Interrupción ADC
    
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128
    
    DIDR0 |= (1 << ADC0D) | (1 << ADC1D) | (1 << ADC2D);  // Deshabilitar entradas digitales
}

// Función readADC original
uint16_t readADC(uint8_t canal) {
    canal &= 0x07;
    ADMUX = (ADMUX & 0xF0) | canal;
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADCH;  // Valor de 8 bits
}

// ISR ADC original
ISR (ADC_vect){
    ADCSRA |= (1 << ADIF);  // Limpiar flag
}