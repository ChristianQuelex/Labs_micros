//UNIVERSIDAD DEL VALLE DE GUATEMALA
//IE2023: PROGRAMACIÓN DE MICROCONTROLADORES
//Pruebas.asm
//AUTOR: Christian Quelex
//Clase#7: Display de 7 segemntos
//HARDWARE: ATMEGA328P
//CREADO: 4/03/2024
//ÚLTIMA MODIFICACIÓN: 

//ENCABEZADO

.INCLUDE "M328PDEF.INC"
.CSEG
.ORG 0X00


//STACK

	LDI R16, LOW(RAMEND)
	OUT SPL, R16
	LDI R17, HIGH(RAMEND)
	OUT SPH, R17


//Tabla de verdad 7 segmentos


T7S: .DB 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71


//CONFIGURACIÓN

SETUP:

	ldi R22, 15		;Tope Maximo
	ldi R24, 0		;Tope minimo
	ldi R16, (1 << CLKPCE)			
	sts CLKPR, R16
	ldi R16, 0b0000_0111
	sts CLKPR, R16

//Configuración de inputs

	//->Habilitar puertos RX Y TX (PD0 Y PD1)
	LDI	R16, 0x00	
	STS	UCSR0B, R16

	ldi R16, 0b0000_1111
	out PORTC, R16			;input pullup (PB2 Y PB1)

	//Configuración Salidas D7S

	ldi R16, 0b0111_1111
	out DDRD, R16			;output display

	ldi R18, 0				;T7S
	ldi R21, 0				; Valor Inicial
	ldi ZH, HIGH(T7S << 1)
	ldi ZL, LOW(T7S << 1)
	add ZL, R18
	lpm R18, Z




LOOP:

	out PORTD, R18


	in R16, PINC
	sbrs R16, PC0			;Si el bit = 1, entonces salta a la siguinte instrucción
	rjmp ANTIRREBOTEB1		;Verifica el antirrebote
	in R17, PINC
	sbrs R17, PC1
	rjmp ANTIRREBOTEB2

	rjmp LOOP


//Subrutinas


ANTIRREBOTEB1:
	//-> Esperar un tiempo breve para el antirrebote (Ajustar si es necesario)
	LDI R16, 100			;Se carga un valor de espera de 100ms
	delayB1:
		DEC R16
		BRNE delayB1		;Regresa a "delayB1" hasta que el valor de R16 sea igual a 0

	//-> Se lee nuevamente el estado del boton despues del antirrebote
	SBIS PINC, PC0			;Salta a la siguiente instrucción si PC0 esta en 1
	RJMP ANTIRREBOTEB1		;Si no esta en 1, llama nuevamente a "ANTIRREBOTEB1" hasta que este en 1
	
	CALL INCREMENTAR		;LLama a la siguiente rutina si PC0 esta en 1

ANTIRREBOTEB2:
	LDI R17, 100			;Se carga un valor de espera de 100ms
	delayB2:
		DEC R17
		BRNE delayB2		;Regresa a "delayB2" hasta que el valor de R17 sea igual a 0

	//-> Se lee nuevamente el estado del boton despues del antirrebote
	SBIS PINC, PC1			;Salta a la siguiente instrucción si PC0 esta en 1
	RJMP ANTIRREBOTEB2		;Si no esta en 1, llama nuevamente a "ANTIRREBOTEB2" hasta que este en 1
	
	CALL DECINCREMENTAR		;LLama a la siguiente rutina si PC1 esta en 1


INCREMENTAR:
	CPSE R21, R22				;Compara los contenidos de R21 y R22, salta a la siguinte instrucción si R21 = R22
	INC R21						;Incrementar D7S
	MOV R18, R21
	LDI ZH, HIGH(T7S << 1)
	LDI ZL, LOW(T7S << 1)
	ADD ZL, R18					
	LPM R18, Z
	RJMP LOOP


DECINCREMENTAR:
	CPSE R21, R24				;Compara los contenidos de R21 y R24, salta a la siguinte instrucción si R21 = R24
	DEC R21
	MOV R18, R21
	LDI ZH, HIGH(T7S << 1)
	LDI ZL, LOW(T7S << 1)
	ADD ZL, R18					
	LPM R18, Z
	RJMP LOOP