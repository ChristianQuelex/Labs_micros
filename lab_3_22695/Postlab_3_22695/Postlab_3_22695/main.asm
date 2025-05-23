;UNIVERSIDAD DEL VALLE DE GUATEMALA
;IE2023: PROGRAMACI?N DE MICROCONTROLADORES
;Laboratorio3.asm
;AUTOR: Christian Quelex
;PROYECTO: 2 DISPLAYS, MUX
;HARDWARE: ATMEGA328P


;ISR_TIMER0

.INCLUDE "M328PDEF.INC"
.CSEG
.ORG 0X00
	JMP SETUP		;RESET VECTOR
.ORG 0X0020
	JMP ISR_TIMER0	;ISR: TIMER0 VECTOR


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

	SBI		DDRB, PB2		// Salidas para multiplexiar 
	SBI		DDRB, PB3

	LDI		R21, 0				// configuracion de lista
	LDI		ZH, HIGH(SEG << 1)
	LDI		ZL, LOW(SEG << 1)
	ADD		ZL, R21
	LPM		R21, Z
	
	CALL	INIT_T0
	SEI							// Habilitar interrupciones

	LDI		R20, 0				// Contador de Timer
	LDI		R21, 0				// Contador de Display
	LDI		R23, 0				// Contador de Decenas

LOOP:
	CBI		PORTB, PB2			// poner a 0 PB4
	CLR		R22					// limpiar R22
	OUT		PORTD, R22			// Presenta
	CBI		PORTB, PB2
	CALL	DISPLAY1			// Mover a Display 1
	SBI		PORTB, PB3			// Encerder PB5
	CBI		PORTB, PB2			// Poner a 0
	OUT		PORTD, R22			//Presentar R22
	

	CBI		PORTB, PB3
	CLR		R24
	OUT		PORTD, R24
	CBI		PORTB, PB3
	CALL	DISPLAY2
	SBI		PORTB, PB2			// Mover a Display 2
	CBI		PORTB, PB3		;MUX
	OUT		PORTD, R24		;IT SHOWS

	CPI		R20, 100			// 100 * 10ms = 1000ms incremento
	BRNE	LOOP
	CLR		R20					// Limpiar R20

	INC		R21
	CPI		R21, 10
	BREQ	ZERO

	JMP		LOOP

ZERO:							// Conador de decenas
	LDI		R21, 0
	INC		R23
	CPI		R23, 6				// Tope
	BREQ	SIX
	RJMP	LOOP

SIX:							// Over de contador 
	LDI		R23, 0
	RJMP	LOOP

DISPLAY1:
	CLR		R22
	MOV		R22, R21
	LDI		ZH, HIGH(SEG << 1)
	LDI		ZL, LOW(SEG << 1)
	ADD		ZL, R22
	LPM		R22, Z
	RET

DISPLAY2:
	CLR		R24
	MOV		R24, R23
	LDI		ZH, HIGH(SEG << 1)
	LDI		ZL, LOW(SEG << 1)
	ADD		ZL, R24
	LPM		R24, Z
	RET

INIT_T0:
	LDI		R16, (1 << CS02) | (1 << CS00)
	OUT		TCCR0B, R16		// prescaler de 1024

	LDI		R16, 99			// valor de overflow
	OUT		TCNT0, R16		// cargar el valor de overflow

	LDI		R16, (1 << TOIE0)
	STS		TIMSK0, R16			// habilitar interrupcion
	RET

ISR_TIMER0:
	LDI		R16, 99			// valor de timer
	OUT		TCNT0, R16		// cargar valor de overflow
	SBI		TIFR0, TOV0		// Apagar bandera
	INC		R20				// Incrementar contador cada 10ms
	RETI