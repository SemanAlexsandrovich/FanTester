/*
 * FanTester.c
 *
 * Created: 07.12.2022 14:44:53
 * Author : sshmykov
 */ 
#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "timer.h"

#define TOP 255
#define PWM_PIN 6
#define LED_PIN 5
#define FAN_NUM 5
#define ACCURATE 0
#define ONE_SEC 1000

volatile uint8_t end_of_time = 0;
uint16_t result[FAN_NUM] = {0};

void port_setup(void) {
	DDRB = 0x00;//enter mode
	DDRD = 0x00;
	DDRB |=  (1 << LED_PIN);
	DDRD |= (1 << PWM_PIN);
	PORTB |= 0b00011111;//Pull-up resistor
	DDRC = 0b00011111;//leds
	PORTC |= 0b00011111;//Pull-up resistor 
}

void setPwmDuty(uint8_t duty) {
	OCR0A = (uint16_t) (duty * TOP) / 100;
}

int main(void) {
	cli();
    port_setup();
	int0_init();
	int1_init();
	setPwmDuty(50);
	sei();
    while (1) {
		for (uint8_t i = 0; i <= FAN_NUM; i++) {
			if ((PINB & (1 << i)) == (1 << i)) {
				result[i]++;
			}
		}
		if (end_of_time) {
			for (uint8_t i = 0; i < FAN_NUM; i++) {
				if (result[i] >= ACCURATE) {
					PORTC |= (1 << i);	
				} 
			}
			_delay_ms(3*ONE_SEC);
			PORTC = 0x00;
			for (uint8_t i = 0; i < FAN_NUM; i++) {
				result[i] = 0;
			}
			end_of_time = 0;
		}
    }
}


