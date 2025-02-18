;UNIVERSIDAD DEL VALLE DE GUATEMALA
;IE2023: PROGRAMACIÓN DE MICROCONTROLADORES
;Laboratorio1.asm
;AUTOR: Christian Quelex
;PROYECTO: Contadores binarios de 4 bits AUTO
;HARDWARE: ATMEGA328P
;CREADO: 06/02/2024
;ÚLTIMA MODIFICACIÓN: 30/01/2024 14:25

.INCLUDE "M328PDEF.INC"
.CSEG
.ORG 0X00

;STACK POINTER SPL SPH
LDI R16, LOW(RAMEND)
OUT SPL, R16
LDI R17, HIGH(RAMEND)
OUT SPH, R17


SETUP:
	
	//LDI		R16, 0x00	
	//STS		UCSR0B, R16

//	LDI		R16, 0xFF
//	OUT		DDRD, R16			
//	LDI     R16, 0x00
//   OUT     PORTD, R16  // Iniciamos en 0

	
//	LDI		R17, 0x03			
//	OUT		PORTC, R17			

	LDI		R16, 0x3F
	OUT		DDRB, R16			
	LDI     R16, 0x00
    OUT     PORTB, R16  // Iniciamos en 0


	CALL	TIMER

	LDI		R20, 0
	LDI		R30, 0

LOOP:
	IN		R16, TIFR0		// lee el registro de banderas de interrupcion
	CPI		R16, (1 << TOV0)	// compara si la bandera de desbordamiento esta activa
	BRNE	LOOP	//espera el desbordamiento 

	CPI		R20, 16		// si R20 es = 16, 0 - 15
	BREQ	ZERO		// realizar salto a zero

	LDI		R16, 100		// carga de valor de overflow
	OUT		TCNT0, R16		// inicar en un valor especifico

	SBI		TIFR0, TOV0		// borra bandera de desbordamiento	

	INC		R30
	CPI		R30, 100
	BRNE	LOOP
	CLR		R30	
	
	INC		R20
	MOV		R25, R20
	OUT		PORTB, R25

	RJMP	LOOP

ZERO:
	LDI		R20, 0		// cargar 0 a R20
	RET

TIMER:
	LDI		R16, (1 << CS02) | (1 << CS00)	// prescaler de 1024
	OUT		TCCR0B, R16

	LDI		R16, 100			// valor de overflow
	OUT		TCNT0, R16			// cargo valor inicial
	RET