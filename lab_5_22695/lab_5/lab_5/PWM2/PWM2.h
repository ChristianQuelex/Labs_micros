/*
 * PWM2.h
 *
 * Created: 11/04/2025 13:25:54
 *  Author: Chris Q
 */ 


#ifndef PWM2_H_
#define PWM2_H_


#include <avr/io.h>
#include <stdint.h>

#define channelA 1
#define channelB 2
#define nop 0
#define yes 1

void initFastPWM2(uint8_t inverted, uint16_t prescaler);
void channel2(uint8_t setChannel, uint8_t inverted);
void convertServo2(uint16_t analogIn, uint8_t selChannel);


#endif /* PWM2_H_ */