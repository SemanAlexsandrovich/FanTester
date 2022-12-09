/*
 * FanTester.c
 *
 * Created: 07.12.2022 14:44:53
 * Author : sshmykov
 */ 
#define F_CPU 16000000L
#include "def.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#ifdef UART_DEBUG
#include "UART.h"
#endif

#include "timer.h"

#define TOP 255
#define PWM_PIN 6
#define LED_PIN 5
#define FAN_NUM 6
#define ACCURATE 0
#define ONE_SEC 1000

#define STATE_MASK 0x3F

volatile uint8_t end_of_time = 0;
uint32_t result[FAN_NUM] = {0};


void port_setup(void) {
	DDRB = 0x00;//enter mode
	DDRD = 0x00;
//	DDRB |=  (1 << LED_PIN);
	DDRD |= (1 << PWM_PIN);
	//PORTB |= 0b00111111;// 0x3F Pull-up resistor
	PORTB = 0xFF;
	DDRC = 0b00111111;//leds
	PORTC = 0; 
}

void setPwmDuty(uint8_t duty) {
	OCR0A = (uint16_t) (duty * TOP) / 100;
}

void dlinyj_test(void) {
	DDRC = 0xFF;//leds
	PORTC = 0;
	_delay_ms(5000);
	while(1) {
		PORTC = 0XFF;
		_delay_ms(1000);
		PORTC = 0;
		_delay_ms(1000);
	}
}

void tachometer_counter() {
	uint8_t old_state = PINB & STATE_MASK;
	uint8_t new_state = 0;
	for (int i = 0; i < FAN_NUM; i++) {
		result[i] = 0;
	}
	
	start_timer1();
	while (0 == end_of_time) {
		new_state = PINB & STATE_MASK;
		for (uint8_t i = 0; i < FAN_NUM; i++) {
			if ((new_state & (1 << i)) != (old_state & (1 << i))) {
				result[i]++;
			}
		}
		old_state = new_state;
	}
	stop_timer1();
	uint8_t mess[] = "Fan# 0 ";
	for (uint8_t i = 0; i < FAN_NUM; i++) {
		mess[5] = 0x30 + i;
		DebagUart_uint32(mess, result[i]);
	}
}

void end_blinking(void) {
	for (int i = 0; i < 4; i++) {
		PORTC=0xFF;
		_delay_ms(500);
		PORTC=0;
		_delay_ms(500);\
	}
}

int main(void) {
	//dlinyj_test();
	
	cli();
	port_setup();
#ifdef UART_DEBUG
	USART_Init();
#endif //UART_DEBUG
	int0_init(); //pwm timer
	int1_init(); 
	setPwmDuty(0);
	sei();
#ifdef UART_DEBUG
	DebagUart("PWM 0\n\r");
#endif //UART_DEBUG
	uint8_t local_state = 0;
	_delay_ms(10 * ONE_SEC);
	tachometer_counter();
	for (uint8_t i = 0; i < FAN_NUM; i++) {
		if (result[i] == 0) {
			local_state |= (1 << i);	
		} 
	}
	DebagUart_uint8("State PWM 0 0x", local_state);
	setPwmDuty(50);
	DebagUart("PWM 50\n\r");
	_delay_ms(10 * ONE_SEC);
	tachometer_counter();
	for (uint8_t i = 0; i < FAN_NUM; i++) {
		if (result[i] < 1000) {
			local_state &= ~ (1 << i);	
		} 
	}
	DebagUart_uint8("State PWM 50 0x", local_state);
	setPwmDuty(0);
	end_blinking();
	PORTC = local_state;
	while(1);
	while (1) {
		for (uint8_t i = 0; i < FAN_NUM; i++) {
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


