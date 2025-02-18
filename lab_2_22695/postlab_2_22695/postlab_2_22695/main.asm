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


T7S: .DB 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71


SETUP:
	
	ldi		R22, 15		;Tope Maximo
	ldi		R24, 0		;Tope minimo


	//->Habilitar puertos RX Y TX (PD0 Y PD1)
	LDI		R16, 0x00	
	STS		UCSR0B, R16

	LDI		R16, 0b11110000
	OUT		DDRC, R16
	LDI		R16, 0b00001111
	OUT		PORTC, R16

	//Configuración Salidas D7S

	LDI		R16, 0b0111_1111
	OUT		DDRD, R16			;output display

	LDI		R18, 0				;T7S
	LDI		R21, 0				; Valor Inicial
	LDI		ZH, HIGH(T7S << 1)
	LDI		ZL, LOW(T7S << 1)
	ADD		ZL, R18
	LPM		R18, Z

	LDI		R16, 0x3F
	OUT		DDRB, R16			
	LDI     R16, 0x00
    OUT     PORTB, R16  // Iniciamos en 0


	CALL	TIMER

	LDI		R20, 0
	LDI		R30, 0

LOOP:

	OUT		PORTD, R18


	SBIS	PINC, PC0
	CALL	DECREMENTAR
	SBIS	PINC, PC1
	CALL	INCREMENTAR

	IN		R26, TIFR0		// lee el registro de banderas de interrupcion
	CPI		R26, (1 << TOV0)	// compara si la bandera de desbordamiento esta activa
	BRNE	LOOP	//espera el desbordamiento 

	CPI		R20, 16		// si R20 es = 16, 0 - 15
	BREQ	ZERO		// realizar salto a zero

	LDI		R26, 100		// carga de valor de overflow
	OUT		TCNT0, R26		// inicar en un valor especifico

	SBI		TIFR0, TOV0		// borra bandera de desbordamiento	

	INC		R30
	CPI		R30, 100
	BRNE	LOOP
	CLR		R30	
	
	INC		R20
	MOV		R25, R20
	OUT		PORTB, R25


	;VER "ALARMA"
	CP R21, R20
	BRNE LOOP
	SBI PINC, PC4		// toggle
	
	MOV R23, R20
	CP R21, R23
	BRNE LOOP
	LDI R20, 0

	RJMP	LOOP



DELAYBOUNCE:
	IN R26, TIFR0
	CPI R26, (1 << TOV0)	;IF FLAG IS NOT SETTED RESTART
	BRNE DELAYBOUNCE
	LDI R26, 100			;LOAD OVERFLOW VALUE AGAIN
	OUT TCNT0, R26
	SBI TIFR0, TOV0			
	INC R30
	CPI R30, 100
	BRNE DELAYBOUNCE
	CLR R30

	SBIS PINC, (PC1 & PC0)	;IT SKIPS NEXT INSTR. IF REGISTER IS 1
	RJMP DELAYBOUNCE
	RJMP LOOP



INCREMENTAR:
	//CALL DELAY
	CPSE	R21, R22				;Compara los contenidos de R21 y R22, salta a la siguinte instrucción si R21 = R22
	INC		R21						;Incrementar D7S
	MOV		R18, R21
	LDI		ZH, HIGH(T7S << 1)
	LDI		ZL, LOW(T7S << 1)
	ADD		ZL, R18					
	LPM		R18, Z
	OUT		PORTD, R18
	RJMP	DELAYBOUNCE


DECREMENTAR:
	//CALL	DELAY
	CPSE	R21, R24				;Compara los contenidos de R21 y R24, salta a la siguinte instrucción si R21 = R24
	DEC		R21
	MOV		R18, R21
	LDI		ZH, HIGH(T7S << 1)
	LDI		ZL, LOW(T7S << 1)
	ADD		ZL, R18					
	LPM		R18, Z
	OUT		PORTD, R18
	RJMP	DELAYBOUNCE


ZERO:
	LDI		R20, 0		// cargar 0 a R20

TIMER:
	LDI		R16, (1 << CS02) | (1 << CS00)	// prescaler de 1024
	OUT		TCCR0B, R16

	LDI		R16, 100			// valor de overflow
	OUT		TCNT0, R16			// cargo valor inicial
	RET