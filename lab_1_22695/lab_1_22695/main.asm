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

.equ	SUBIR = PD2
.equ	BAJAR = PD3
.equ	SUBIR1 = PB4
.equ	BAJAR1 = PB5
// Configurar el MCU
SETUP:
    // Configurar pines de entrada y salida (DDRx, PORTx, PINx)
    // PORTD como entrada con pull-up habilitado
    LDI     R16, 0b11110000
    OUT     DDRD, R16   // Setear puerto D como entrada
    LDI     R16, 0b00001111
    OUT     PORTD, R16  // Habilitar pull-ups en puerto D

    // PORTB como salida inicialmente encendido
    LDI     R16, 0b00001111
    OUT     DDRB, R16   // Setear puerto B como salida
    LDI     R16, 0b11110000
    OUT     PORTB, R16  // Encender primer bit de puerto B

	// PRESCALER
	LDI		R22, (1 << CLKPCE)
	STS		CLKPR, R22
	LDI		R22, 0b0000_0100
	STS		CLKPR, R22


	// PORTC como salidas
	LDI		R16, 0xFF
	OUT		DDRC, R16	//setear puerto C como salida
	LDI		R16, 0x00
	OUT		PORTC, R16	// Encender todos los bits del pueto C


	// VALORES DE SALIDAS
    LDI     R17, 0x00  // Contador 1
	LDI		R28, 0x00  // Contador 2
	LDI		R20, 0


// Loop infinito
MAIN:
	//OUT		PORTB, R17
	//OUT		PORTC, R20
    SBIS    PIND, SUBIR      // si pd2 es 0 saltar
    CALL	INCREMENTO1		// ir a incremento
	SBIS	PIND, BAJAR		// si pd3 es 0 saltar
	CALL    DECREMENTO1
	//SBIS	PINB, BAJAR1	// lee el puerto pb4
	//CALL	INCREMENTO2		
	SBIS    PINB, SUBIR1	// lee el puerto pb5 
	CALL	DECREMENTO2
	
	     // Toggle de PB0
    RJMP    MAIN


INCREMENTO1:
	//CALL	DELAY
	INC		R17
	OUT		PORTB, R17
	RJMP	DELAY
	RJMP	MAIN

DECREMENTO1:
	//CALL	DELAY
	DEC		R17
	OUT		PORTB, R17
	RJMP	DELAY
	RJMP	MAIN 

INCREMENTO2:
	//CALL	DELAY
	INC		R28
	OUT		PORTC, R28
	RJMP	DELAY
	RJMP	MAIN
	//RET
DECREMENTO2:
	//CALL	DELAY
	DEC		R28
	OUT		PORTC, R28
	RJMP	DELAY
	RJMP	MAIN
	//RET
// Sub-rutina (no de interrupción)
DELAY:
    LDI     R18, 0
	LDI		R19, 0
BUCLE:
	INC		R18
	CPI		R18, 0
	BRNE	BUCLE
	INC		R19
	CPI		R19, 0
	BRNE	BUCLE
    RET


// Rutinas de interrupción
