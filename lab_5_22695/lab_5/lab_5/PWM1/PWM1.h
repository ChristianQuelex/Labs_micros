/*
 * PWM1.h
 *
 * Created: 11/04/2025 01:16:14 a. m.
 *  Author: Christian Quelex
 */ 


#ifndef PWM1_H_
#define PWM1_H_

#include <avr/io.h>
#include <stdint.h>

#define yes 1
#define nop 0		// para inversiones de signal 
#define normal 1
#define settedUp 2	// para el modo establecido 
#define channelA 1
#define channelB 2	// para seleccionar el canal de salida 

/* Use it before setChannel();
Settings for Fast PWM 1, 16 bits
(modePWM <- normal/settedUp, prescaler <- 1,8,64,256,1024)
*/
void initFastPWM1(uint8_t modePWM, uint16_t prescaler);
//modo normal o settedUp - en este casio 8 o 1024

//(channel <- channelA/channelB, inverted <- yes/nop)
void channel(uint8_t setChannel, uint8_t inverted);
// para ver si el canal esta invertido o no 

//topValue, used if mode is settedUp
void topValue(uint16_t top);

/*conversion for servos, mapping values
(analogIN = ADCH, channelA/channelB)*/
void convertServo(uint16_t analogIn, uint8_t selChannel);


#endif /* PWM1_H_ */