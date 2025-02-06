/*
* Antirebote.asm
*
* Creado: 29-Jan-25 5:25:03 PM
* Autor : Pedro Castillo
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

    LDI     R17, 0xFF   // Variable para guardar estado de botones
	ldi r24 , 0
// Loop infinito
MAIN:
    IN      R16, PIND   // Guardando el estado de PORTD en R16  0xFF
    CP      R17, R16    // Comparamos estado "viejo" con estado "nuevo"
    BREQ    MAIN
    CALL    DELAY
    IN      R16, PIND
    CP      R17, R16
    BREQ    MAIN
    // Volver a leer PIND
    MOV     R17, R16
    SBRS    R16, 2      // Salta si el bit 2 del PIND es 1 (no apachado)
    inc     r24

	sbrs r16, 1
	dec r24

	out portb, r24
	     // Toggle de PB0
    RJMP    MAIN

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
