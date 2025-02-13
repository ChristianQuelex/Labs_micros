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

.equ	SUBIR = PC3
.equ	BAJAR = PC2
.equ	SUBIR1 = PC1
.equ	BAJAR1 = PC0
.equ	SUMAR = PC4
// Configurar el MCU
SETUP:
    // Configurar pines de entrada y salida (DDRx, PORTx, PINx)
    // PORTD como entrada con pull-up habilitado

	LDI	R16, 0x00	
	STS	UCSR0B, R16

    LDI     R16, 0xFF
    OUT     DDRD, R16   // Setear puerto D como entrada
    LDI     R16, 0x00
    OUT     PORTD, R16  // Habilitar pull-ups en puerto D

    // PORTB como salida inicialmente encendido
    LDI     R16, 0xFF
    OUT     DDRB, R16   // Setear puerto B como salida
    LDI     R16, 0x00
    OUT     PORTB, R16  // Encender primer bit de puerto B

	// PRESCALER
	LDI		R22, (1 << CLKPCE)
	STS		CLKPR, R22
	LDI		R22, 0b0000_0100
	STS		CLKPR, R22


	// PORTC como salidas
	LDI		R16, 0x00
	OUT		DDRC, R16	//setear puerto C como salida
	LDI		R16, 0xFF
	OUT		PORTC, R16	// Encender todos los bits del pueto C


	// VALORES DE SALIDAS
    LDI     R17, 0x00  // Contador 1
	LDI		R20, 0x00  // Contador 2
	


// Loop infinito
MAIN:
    SBIS    PINC, SUBIR      // si pd2 es 0 saltar
    CALL	INCREMENTO1		// ir a incremento
	SBIS	PINC, BAJAR		// si pd3 es 0 saltar
	CALL    DECREMENTO1
	SBIS	PINC, SUBIR1	// lee el puerto pb4
	CALL	INCREMENTO2		
	SBIS    PINC, BAJAR1	// lee el puerto pb5 
	CALL	DECREMENTO2
	SBIS	PINC, SUMAR
	CALL	SUMANDO
    RJMP    MAIN


INCREMENTO1:
	CALL	DELAY
	INC		R17
	CALL	UNION
	RET

DECREMENTO1:
	CALL	DELAY
	DEC		R17
	CALL	UNION
	RET 

INCREMENTO2:
	CALL	DELAY
	INC		R20
	CALL	UNION
	RET

DECREMENTO2:
	CALL	DELAY
	DEC		R20
	CALL	UNION
	RET

UNION:
    ANDI	R17, 0x0F   // Utilizar unicamente los primeros 4 bits
    ANDI	R20, 0x0F    
    SWAP	R20			//Cambiar los primeros bits por los ultimos
	MOV		R21, R17
    OR		R21, R20	// Unir ambos registros
	SWAP	R20			// Regresar registro a su estado original
    OUT		PORTD, R21  // Presentar
	RET

SUMANDO:
	CALL	DELAY
	MOV		R23, R17
	ADD		R23, R20
	ANDI	R23, 0x1F	// Habilito overflow
	OUT		PORTB, R23
	RET


// Sub-rutina (no de interrupción)
DELAY:
    LDI     R18, 0x00
	LDI		R19, 0x00
BUCLE:
	INC		R18
	CPI		R18, 0
	BRNE	BUCLE
	INC		R19
	CPI		R19, 0
	BRNE	BUCLE
    RET

// Rutinas de interrupción
