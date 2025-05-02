/*
* Laboratorio6.c
*
* Created: 4/22/2024
* Author : Christian Quelex
*
*/

#define F_CPU 16000000UL

#define ASCII_0  '0'

// variables
#define MENU_OPCION1  '1'
#define MENU_OPCION2  '2'
#define SALTO_LINEA  10
#define RETORNO_CARRO 13
#define TIEMPO_ESPERA 30		// variable para delay

// librerias 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Funciones 
void ConfigurarUART9600(void);		// void para interrupciones 
void ConfigurarADC(void);
void EnviarCaracterUART(char caracter);		//Funciones para caracteres
void EnviarTextoUART(char *texto);			//Funciones para cadenas
void ProcesarValorADC(uint8_t valor);		//Funciones para presentaciones del micro 

// Variables globales
volatile uint8_t datoRecibido = 0;
volatile uint8_t opcionSeleccionada = 0;
volatile uint8_t valorPotenciometro = 0;
volatile uint8_t estadoPrograma = 0; // 0=espera, 1=menu, 2=adc, 3=ascii

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
			// estadoPrograma con valor 0 
			case 0: // Estado de espera
			// No hacer nada, esperar interrupción
			// aca permanece 
			break;
			
			case 1: // Mostrar menú
			EnviarCaracterUART(SALTO_LINEA);		// valor 10
			EnviarTextoUART("Seleccione una opcion:");
			EnviarCaracterUART(SALTO_LINEA);
			EnviarTextoUART("1. Leer Potenciometro");
			EnviarCaracterUART(SALTO_LINEA);
			EnviarTextoUART("2. Enviar caracter ASCII");
			EnviarCaracterUART(SALTO_LINEA);
			
			estadoPrograma = 0; // Volver a espera
			break;
			//reinicia- vuelve a 0
			
			case 2: // Procesar ADC
			ADCSRA |= (1 << ADSC); // Iniciar conversión
			estadoPrograma = 0; // Volver a espera
			_delay_ms(TIEMPO_ESPERA);
			break;
			
			case 3: // Esperar ASCII
			EnviarCaracterUART(SALTO_LINEA);
			EnviarTextoUART("Ingrese caracter ASCII:");
			EnviarCaracterUART(SALTO_LINEA);
			estadoPrograma = 0; // Volver a espera
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
	// comunicacion 
	UBRR0 = 103;
}

void ConfigurarADC(void)
{
	// Configurar ADC
	ADMUX = (1 << REFS0) | (1 << ADLAR); // Referencia AVCC, ajuste izquierdo
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Habilitar ADC, interrupción y prescaler 128
	// lectura correcta del ADC 
	
	// Deshabilitar entrada digital para PC0 (ADC0) - necesario para utilizarlo 
	DIDR0 |= (1 << ADC0D);
}

	// Sirve para esperar a que se envie todo 
void EnviarCaracterUART(char caracter)
{
	while(!(UCSR0A & (1<<UDRE0))); // Esperar buffer de transmisión vacío
	UDR0 = caracter;
}
	// Cnfiguraciones para textos 
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
	//Configuracion matematica 
	uint8_t unidades = valor % 10;
	uint8_t decenas = (valor / 10) % 10;
	uint8_t centenas = (valor / 100) % 10;
	
	// Enviar texto de Valor
	EnviarTextoUART("Valor Potenciometro: ");
	EnviarCaracterUART(ASCII_0 + centenas);
	EnviarCaracterUART(ASCII_0 + decenas);
	EnviarCaracterUART(ASCII_0 + unidades);
	EnviarCaracterUART(SALTO_LINEA);
	EnviarCaracterUART(RETORNO_CARRO);
	
	// Presentar el valor en puertos 
	PORTD = (valor & 0x3F) << 2;  // 6 bits menos significativos en PD5-PD0
	PORTB = (valor >> 6) & 0x03;  // 2 bits más significativos en PB1-PB0
}

// Interrupción por recepción UART
ISR(USART_RX_vect)
{
	datoRecibido = UDR0;		// Transmisor - receptor
	// ignorar caracteres de control (salto y lineas de carro)
	if (datoRecibido == SALTO_LINEA || datoRecibido == RETORNO_CARRO)
	{	return;
	}
	
	
	if(estadoPrograma == 0) {
		if(datoRecibido == MENU_OPCION1) {
			opcionSeleccionada = MENU_OPCION1;			// Variable con valor 1
			estadoPrograma = 2; // Estado para procesar ADC
		}
		else if(datoRecibido == MENU_OPCION2) {
			opcionSeleccionada = MENU_OPCION2;			// Variable con valor 2 
			estadoPrograma = 3; // Estado para recibir ASCII
			//while(UCSR0A & (1 << RXEN0)) {UDR0;}
			//	EnviarTextoUART("\n Ingrese un caracter: ")
		}
		else {
			estadoPrograma = 1; // Mostrar menú de nuevo
		}
	}
	else if(estadoPrograma == 3) {
		
		
		
		// Mostrar caracter ASCII recibido
		EnviarTextoUART("Caracter recibido: ");
		EnviarCaracterUART(datoRecibido);
		EnviarCaracterUART(SALTO_LINEA);
		
		// Mostrar en puertos
		PORTD = (datoRecibido & 0x3F) << 2;  // 6 bits menos significativos
		PORTB = (datoRecibido >> 6) & 0x03;  // 2 bits más significativos
		
		estadoPrograma = 1; // Volver al menú
	}
}

// Interrupción por fin de conversión ADC
ISR(ADC_vect)
{
	// ADCH para convercion-division de valores mas significativos y menos
	valorPotenciometro = ADCH; // Leer resultado de 8 bits (ADLAR=1)
	ProcesarValorADC(valorPotenciometro);
	
	ADCSRA |= (1 << ADIF); // Limpiar bandera de interrupción
	estadoPrograma = 1; // Volver al menú
}