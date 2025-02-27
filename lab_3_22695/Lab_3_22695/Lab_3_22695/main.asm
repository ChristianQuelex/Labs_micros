;UNIVERSIDAD DEL VALLE DE GUATEMALA
;IE2023: PROGRAMACI?N DE MICROCONTROLADORES
;Laboratorio3.asm
;AUTOR: Christian Quelex
;PROYECTO: Contador binario de 4 bits
;HARDWARE: ATMEGA328P
;CREADO: 30/01/2024


;ISR_TIMER0

.INCLUDE "M328PDEF.INC"
.CSEG
.ORG 0X00
	JMP SETUP			//RESET VECTOR

.ORG 0X0008
	JMP ISR_PCINT1

.ORG 0X0020
	JMP ISR_TIMER0		//ISR: Activacion de desbordamiento

MAIN:

	LDI		R16, LOW(RAMEND)
	OUT		SPL, R16
	LDI		R17, HIGH(RAMEND)
	OUT		SPH, R17

	SEG: .DB 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71

SETUP:
	LDI		R16, 0x00		// deshabilitar puertos usart
	STS		UCSR0B, R16
	
	LDI		R16, 0xFF		// Salidas puertos D
	OUT		DDRD, R16
	LDI		R16, 0x00
	OUT		PORTD, R16

	LDI		R21, 0
	LDI		ZH, HIGH(SEG << 1)
	LDI		ZL, LOW(SEG << 1)
	ADD		ZL, R21
	LPM		R21, Z

	LDI		R16, 0XFF		//Habilitar puertos B 
	OUT		DDRB, R16
	LDI		R16, 0X00
	OUT		PORTB, R16

	LDI		R16, 0X00		// Entradas en el puerto C
	OUT		DDRC, R16
	LDI		R16, 0b00111111		//Activacion de Pull-up
	OUT		PORTC, R16


	LDI		R16, (1 << PCINT8) | (1 << PCINT9)	// Activacion de puetos PC0 y PC1
	STS		PCMSK1, R16

	LDI		R16, (1 << PCIE1)
	STS		PCICR, R16	

	// R20 CONTADOR
	// R21 DISPLAY

	
	CALL	INIT_T0

	SEI						// habilitar interrupciones

	LDI		R20, 0			// Contador de timer
	LDI		R21, 0			// Contador de Display
	LDI		R28, 0			// Registro para Leds



LOOP:
	
	OUT		PORTB, R28

	
	CPI		R20, 99			; 100 * 10ms = 1000ms
	BRNE	LOOP
	CLR		R20				//Limpiar R20 
	
	CALL	DISPLAY

	INC		R21
	SBRC	R21, 4		// Resetear a 0
	LDI		R21, 0

	JMP		LOOP


ISR_PCINT1:
	PUSH	R16			//Guarda valores de registro y banderas 
	IN		R16, SREG
	PUSH	R16

	IN		R18, PINC

	SBRC	R18, PC0		// Lectura del pin PC0, si es 0 saltar
	JMP		CHECKPC1
	CALL	DELAY
	INC		R28
	CPI		R28, 16			// Limite de contador
	BRNE	EXIT
	LDI		R28, 0
	JMP		EXIT

CHECKPC1:
	SBRC	R18, PC1
	JMP		EXIT
	CALL	DELAY
	DEC		R28
	CPI		R28, 0XFF
	BRNE	EXIT
	LDI		R28, 15


EXIT:
	SBI		PCIFR, PCIF1	// Apaga la bandera de interrupcion del puerto

	POP		R16				// Restaura valores de registros y Banderas
	OUT		SREG, R16
	POP		R16
	RETI

DELAY:
    LDI     R24, 0x00
	LDI		R25, 0x00
	LDI		R26, 0xF0
BUCLE:
	INC		R24
	CPI		R24, 0
	BRNE	BUCLE
	INC		R25
	CPI		R25, 0
	BRNE	BUCLE
	INC		R26
	CPI		R26, 0
	BRNE	BUCLE
    RET

DISPLAY:
	CLR		R22
	MOV		R22, R21
	LDI		ZH, HIGH(SEG << 1)
	LDI		ZL, LOW(SEG << 1)
	ADD		ZL, R22
	LPM		R22, Z
	
	OUT		PORTD, R22

	RET


INIT_T0:				//Desbordamiento para cada 10 ms 
	LDI		R16, (1 << CS02) | (1 << CS00)
	OUT		TCCR0B, R16			// Prescaler de 1024

	LDI		R16, 99			// varlor de overflow
	OUT		TCNT0, R16		// Cargar el inicio para el timer

	LDI		R16, (1 << TOIE0)	// TOIE0 interrupcion por desbordamiento 
	STS		TIMSK0, R16			// habilitamos interrupcion 
	RET

ISR_TIMER0:
	LDI		R16, 99			// valor del timer para el overflow
	OUT		TCNT0, R16		// cargar valor de overflow - desbordamiento 
	SBI		TIFR0, TOV0		// apagar bandera
	INC		R20				// incrementar cada 10ms 
	RETI					// Regreso de Rutina de interrupcion
