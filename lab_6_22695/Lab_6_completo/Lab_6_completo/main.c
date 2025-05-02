/*
* Laboratorio6.c
*
* Created: 4/22/2024
* Author : Christian Quelex
*
*/

#define F_CPU 16000000UL

#define ASCII_0  '0'

// Constantes
#define MENU_OPCION1  '1'
#define MENU_OPCION2  '2'
#define SALTO_LINEA  10
#define RETORNO_CARRO 13
#define TIEMPO_ESPERA 30

// Librerías
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Prototipos de funciones
void ConfigurarUART9600(void);
void ConfigurarADC(void);
void EnviarCaracterUART(char caracter);
void EnviarTextoUART(char *texto);
void ProcesarValorADC(uint8_t valor);

// Variables globales
volatile uint8_t datoRecibido = 0;
volatile uint8_t opcionSeleccionada = 0;
volatile uint8_t valorPotenciometro = 0;
volatile uint8_t estadoPrograma = 0;  // 0=espera, 1=menu, 2=adc
volatile uint8_t esperandoCaracter = 0;  // Bandera para recepción de caracteres

int main(void)
{
	// Inicialización de puertos
	DDRB = 0xFF;  // Puerto B como salida
	DDRD = 0xFF;  // Puerto D como salida
	
	// Configurar periféricos
	ConfigurarUART9600();
	ConfigurarADC();
	
	// Habilitar interrupciones globales
	sei();
	
	// Mensaje inicial
	EnviarTextoUART("\nSistema Iniciado\n");
	
	while (1)
	{
		switch(estadoPrograma) {
			case 0: // Estado de espera
			// No hacer nada, esperar interrupción
			break;
			
			case 1: // Mostrar menú
			EnviarCaracterUART(SALTO_LINEA);
			EnviarTextoUART("Seleccione una opcion:");
			EnviarCaracterUART(SALTO_LINEA);
			EnviarTextoUART("1. Leer Potenciometro");
			EnviarCaracterUART(SALTO_LINEA);
			EnviarTextoUART("2. Enviar caracter ASCII");
			EnviarCaracterUART(SALTO_LINEA);
			estadoPrograma = 0; // Volver a espera
			break;
			
			case 2: // Procesar ADC
			ADCSRA |= (1 << ADSC); // Iniciar conversión
			estadoPrograma = 0; // Volver a espera
			_delay_ms(TIEMPO_ESPERA);
			break;
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

void ConfigurarADC(void)
{
	// Configurar ADC
	ADMUX = (1 << REFS0) | (1 << ADLAR); // Referencia AVCC, ajuste izquierdo
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Habilitar ADC, interrupción y prescaler 128
	
	// Deshabilitar entrada digital para PC0 (ADC0)
	DIDR0 |= (1 << ADC0D);
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

void ProcesarValorADC(uint8_t valor)
{
	uint8_t unidades = valor % 10;
	uint8_t decenas = (valor / 10) % 10;
	uint8_t centenas = (valor / 100) % 10;
	
	EnviarTextoUART("\nValor Potenciometro: ");
	EnviarCaracterUART(ASCII_0 + centenas);
	EnviarCaracterUART(ASCII_0 + decenas);
	EnviarCaracterUART(ASCII_0 + unidades);
	EnviarCaracterUART(SALTO_LINEA);
	
	PORTD = (valor & 0x3F) << 2;
	PORTB = (valor >> 6) & 0x03;
}

// Interrupción por recepción UART - Versión definitiva
ISR(USART_RX_vect)
{
	datoRecibido = UDR0;
	
	// Ignorar siempre caracteres de control
	if(datoRecibido == SALTO_LINEA || datoRecibido == RETORNO_CARRO) {
		return;
	}

	// Modo recepción de caracteres activado
	if(esperandoCaracter) {
		esperandoCaracter = 0; // Desactivar bandera
		
		// Mostrar confirmación
		EnviarTextoUART("\nCaracter recibido: ");
		EnviarCaracterUART(datoRecibido);
		EnviarTextoUART("\n");
		
		// Mostrar en LEDs
		PORTD = (datoRecibido & 0x3F) << 2;
		PORTB = (datoRecibido >> 6) & 0x03;
		
		// Volver al menú
		estadoPrograma = 1;
		return;
	}

	// Procesamiento de opciones del menú
	if(estadoPrograma == 0) {
		if(datoRecibido == MENU_OPCION1) {
			estadoPrograma = 2; // Leer ADC
		}
		else if(datoRecibido == MENU_OPCION2) {
			// Activar modo recepción de caracteres
			esperandoCaracter = 1;
			EnviarTextoUART("\nIngrese caracter ASCII: ");
		}
		else {
			// Opción no válida
			estadoPrograma = 1; // Mostrar menú
		}
	}
}

// Interrupción por fin de conversión ADC
ISR(ADC_vect)
{
	valorPotenciometro = ADCH; // Leer resultado de 8 bits (ADLAR=1)
	ProcesarValorADC(valorPotenciometro);
	
	ADCSRA |= (1 << ADIF); // Limpiar bandera de interrupción
	estadoPrograma = 1; // Volver al menú
}
