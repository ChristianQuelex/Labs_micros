/*
* Antirebote.asm
*
* Creado: 29-Jan-25 5:25:03 PM
* Autor : CHRISTIAN QUELEX
* Descripción: 
*/
// Encabezado
.include "M328PDEF.inc"
.cseg
.org    0x0000
// Configurar la pila
LDI     R16, LOW(RAMEND)
OUT     SPL, R16
LDI     R16, HIGH(RAMEND)
OUT     SPH, R16

// Configurar el MCU
SETUP:
    // Configurar pines de entrada y salida (DDRx, PORTx, PINx)
    // PORTD como entrada con pull-up habilitado
    LDI     R16, 0x00
    OUT     DDRD, R16   // Setear puerto D como entrada
    LDI     R16, 0xFF
    OUT     PORTD, R16  // Habilitar pull-ups en puerto D

    // PORTB como salida inicialmente encendido
    LDI     R16, 0xFF
    OUT     DDRB, R16   // Setear puerto B como salida
    LDI     R16, 0b00000001
    OUT     PORTB, R16  // Encender primer bit de puerto B

	// PORTC como salidas
	LDI		R16, 0xFF
	OUT		DDRC, R16	//setear puerto C como salida
	LDI		R16, 0xFF
	OUT		PORTC, R16	// Encender todos los bits del pueto C


	// VALORES DE SALIDAS
    LDI     R17, 0x00   // valor inicial R17
	LDI		R19, 0x00	// valor inicial R18
	LDI		R24 , 0		// valor inicial R24


// Loop infinito
MAIN:
    OUT		PORTB, R17
	OUT		PORTC, R18
    SBIC    PIND, PD2      // Lea el puerto pd2
    CALL	INCREMENTO1		// si es 1 llame a incremento
	SBIC	PIND, PD3		// si no es 1 lea esta instruccion
	CALL    DECREMENTO1
	SBIC	PIND, PD4
	CALL	INCREMENTO2
	SBIC	PIND, PD5
	CALL	DECREMENTO2
	
	     // Toggle de PB0
    RJMP    MAIN


INCREMENTO1:
	CALL	DELAY
	INC		R17
	OUT		PORTB, R17
	RJMP	MAIN

DECREMENTO1:
	CALL	DELAY
	DEC		R17
	OUT		PORTB, R17
	RJMP	MAIN 

INCREMENTO2:
	CALL	DELAY
	INC		R19
	OUT		PORTC, R19
	RJMP	MAIN

DECREMENTO2:
	CALL	DELAY
	INC		R19

// Sub-rutina (no de interrupción)
DELAY:
    LDI     R18, 0xFF
SUB_DELAY1:
    DEC     R18
    CPI     R18, 0
    BRNE    SUB_DELAY1
    LDI     R18, 0xFF
SUB_DELAY2:
    DEC     R18
    CPI     R18, 0
    BRNE    SUB_DELAY2
    LDI     R18, 0xFF
SUB_DELAY3:
    DEC     R18
    CPI     R18, 0
    BRNE    SUB_DELAY3
    RET


// Rutinas de interrupción
