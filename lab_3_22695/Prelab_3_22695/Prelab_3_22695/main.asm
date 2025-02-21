;UNIVERSIDAD DEL VALLE DE GUATEMALA
;IE2023: PROGRAMACIÓN DE MICROCONTROLADORES
;Laboratorio3.asm
;AUTOR: Christian Quelex
;PROYECTO: Contador con interrupciones 
;HARDWARE: ATMEGA328P
;CREADO: 30/01/2024

;ISR_TIMER0

;ISR_PCINT

.INCLUDE "M328PDEF.INC"
.CSEG
.ORG 0X00
	JMP SETUP			// RESET VECTOR

.ORG 0X0008
	JMP ISR_PCINT1		// ISR: Vector de direccion puertos C

SETUP:
	;STACK
	LDI		R16, LOW(RAMEND)
	OUT		SPL, R16
	LDI		R17, HIGH(RAMEND)
	OUT		SPH, R17



	LDI		R16, 0XFF		// Puertos B como salidas
	OUT		DDRB, R16
	LDI		R16, 0x00
	OUT		PORTB, R16

	LDI		R16, 0XFF		// Puertos D como salidas
	OUT		DDRD, R16

	LDI		R16, 0X00		// Entradas en el puerto C
	OUT		DDRC, R16
	LDI		R16, 0b00111111		//Activacion de Pull-up
	OUT		PORTC, R16

	LDI		R16, (1 << PCINT8) | (1 << PCINT9)	// Activacion de puetos PC0 y PC1
	STS		PCMSK1, R16

	LDI		R16, (1 << PCIE1)
	STS		PCICR, R16			// Cargar interrupcion en los puertos C
	
	SEI
	LDI		R20, 0X00 

LOOP:
	OUT		PORTB, R20		//SHOW COUNT   0b0000_0100
	RJMP	LOOP

ISR_PCINT1:
	PUSH	R16			//Guarda valores de registro y banderas 
	IN		R16, SREG
	PUSH	R16

	IN		R18, PINC

	SBRC	R18, PC0		// Lectura del pin PC0, si es 0 saltar
	JMP		CHECKPC1
	CALL	DELAY
	INC		R20
	CPI		R20, 16			// Limite de contador
	BRNE	EXIT
	LDI		R20, 0
	JMP		EXIT

CHECKPC1:
	SBRC	R18, PC1
	JMP		EXIT
	CALL	DELAY
	DEC		R20
	CPI		R20, 0XFF
	BRNE	EXIT
	LDI		R20, 15

EXIT:
	SBI		PCIFR, PCIF1	// Apaga la bandera de interrupcion del puerto

	POP		R16				// Restaura valores de registros y Banderas
	OUT		SREG, R16
	POP		R16
	RETI

DELAY:
    LDI     R24, 0x00
	LDI		R25, 0x00
	LDI		R26, 0xEE
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
