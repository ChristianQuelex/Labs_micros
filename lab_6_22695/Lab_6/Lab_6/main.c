/*
* ComunicacionUART_PD2-PD7.c
*
* Muestra datos recibidos en:
* - PD7 a PD2 (6 bits menos significativos)
* - PB1 y PB0 (2 bits más significativos)
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Prototipos
void ConfigurarUART9600(void);
void EnviarCaracterUART(char c);
void cadena(char txt[]);
void MostrarEnPuertos(uint8_t dato);

volatile uint8_t datoRecibido = 0;

int main(void)
{
	// Configurar puertos (PB y PD como salidas)
	DDRB = 0xFF;
	DDRD = 0xFF;
	
	// Inicializar UART
	ConfigurarUART9600();
	sei();
	
	// Mensaje inicial
	cadena("\nSistema UART - Visualizacion en PD2-PD7\n");
	cadena("Envie caracteres para verlos en:\n");
	cadena("- PB1-PB0: 2 bits mas significativos\n");
	cadena("- PD7-PD2: 6 bits menos significativos\n\n");
	
	while(1)
	{
		if(datoRecibido)
		{
			MostrarEnPuertos(datoRecibido);
			
			// Confirmación del dato recibido
			cadena("Caracter recibido: ");
			EnviarCaracterUART(datoRecibido);
			cadena(" (0x");
			// Mostrar valor hexadecimal
			uint8_t nibble = (datoRecibido >> 4) & 0x0F;
			EnviarCaracterUART(nibble > 9 ? nibble - 10 + 'A' : nibble + '0');
			nibble = datoRecibido & 0x0F;
			EnviarCaracterUART(nibble > 9 ? nibble - 10 + 'A' : nibble + '0');
			cadena(")\n");
			
			datoRecibido = 0;
		}
		_delay_ms(10);
	}
}

void ConfigurarUART9600(void)
{
	DDRD &= ~(1<<DDD0);  // PD0 (RX) como entrada
	DDRD |= (1<<DDD1);   // PD1 (TX) como salida
	UBRR0 = 103;         // 9600 bps a 16MHz
	UCSR0A = 0;
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

void EnviarCaracterUART(char c)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
}

void cadena(char txt[])
{
	uint8_t i = 0;
	while(txt[i] != '\0')
	{
		EnviarCaracterUART(txt[i]);
		i++;
	}
}

// Función modificada para PD2-PD7
void MostrarEnPuertos(uint8_t dato)
{
	// PD7-PD2 = bits [5:0] del dato (6 bits LSB)
	PORTD = (dato & 0x3F) << 2;
	
	// PB1-PB0 = bits [7:6] del dato (2 bits MSB)
	PORTB = (dato >> 6) & 0x03;
	
	// Mantener PD1 (TX) y PD0 (RX) sin cambios
	PORTD |= (1<<PD1);  // Mantener TX en estado alto
}

ISR(USART_RX_vect)
{
	datoRecibido = UDR0;
}