/*
* Laboratorio6.c
*
* Created: 4/25/2025
* Author : Christian Quelex
*
* ComunicacionUART_Visualizacion.c
*
*/

#define F_CPU 16000000UL		// frecuencia 
#define SALTO_LINEA  10			// Codigo ascci para control de terminal
#define RETORNO_CARRO 13


//funciones al utilizar
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Prototipos de funciones
void ConfigurarUART9600(void);
void EnviarCaracterUART(char caracter);
void EnviarTextoUART(char *texto);
void MostrarEnPuertos(uint8_t dato);

// Variable global para dato recibido
volatile uint8_t datoRecibido = 0;

int main(void)
{
	// Configurar puertos como salida
	DDRB = 0xFF;  // Puerto B completo como salida
	DDRD = 0xFF;  // Puerto D completo como salida
	
	// Configurar UART - llamar funcion 
	ConfigurarUART9600();
	
	// Habilitar interrupciones globales
	sei();
	
	// Mensaje inicial
	EnviarTextoUART("\nInicio de Transmision \n");
	
	
	while (1)
	{
		if(datoRecibido != 0) {
			// Mostrar el dato recibido en los puertos
			MostrarEnPuertos(datoRecibido);
			
			// Opcional: Enviar confirmación por UART
			EnviarTextoUART("Caracter mostrado en puertos: ");
			EnviarCaracterUART(datoRecibido);
			EnviarCaracterUART(SALTO_LINEA);
			
			datoRecibido = 0; // Resetear el dato
		}
	}
}

void ConfigurarUART9600(void)
{
	// Configurar pines: PD0(RX) como entrada, PD1(TX) como salida
	DDRD &= ~(1<<DDD0);
	DDRD |= (1<<DDD1);
	
	// Configurar UART
	UCSR0A = 0; // Limpiar registro de estado
	UCSR0B = (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0); // Habilitar interrupciones RX, TX y RX
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); // Modo 8 bits, sin paridad, 1 bit de stop
	
	// Configurar baud rate (9600 a 16MHz)
	UBRR0 = 103;
}

void EnviarCaracterUART(char caracter)
{
	while(!(UCSR0A & (1<<UDRE0))); // Esperar buffer de transmisión vacío
	UDR0 = caracter;
}

void EnviarTextoUART(char *texto)
{
	uint8_t i = 0;
	while(texto[i] != '\0') {
		EnviarCaracterUART(texto[i]);
		i++;
	}
}

void MostrarEnPuertos(uint8_t dato)
{
	// Mostrar en puertos según tu configuración original:
	PORTD = (dato & 0x3F) << 2;  // 6 bits menos significativos en PD7-PD2 (desplazados 2 posiciones)
	PORTB = (dato >> 6) & 0x03;  // 2 bits más significativos en PB1-PB0
}

// Interrupción por recepción UART
ISR(USART_RX_vect)
{
	datoRecibido = UDR0; // Almacenar el dato recibido
}