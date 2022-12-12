/*
 * timer.c
 *
 * Created: 07.12.2022 16:09:22
 *  Author: sshmykov
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer.h"

#define TIME 5

extern volatile uint8_t end_of_time;

void int0_init(void) {
	//pwm
	TCCR0A |= (1 << WGM00) | (1 << COM0A1);
	TCCR0B |= (1 << WGM01) | (1 << CS00);
	TCNT0 = 0;

}

void start_timer1(void) {
	TCNT1 = 0;
	end_of_time = 0;
	TCCR1B |= (1 << CS12) | (1 << CS10); //set prescaler
	TIMSK1 |= (1 << OCIE1A); //enable interrupt
}

void stop_timer1(void) {
	TIMSK1 &= ~(1 << OCIE1A);
	TCCR1B &= ~((1 << CS12) | (1 << CS10));
}

void reset_timer1(void) {
	TCNT1 = 0;
	end_of_time = 0;
}

void int1_init(void) {
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	OCR1A = 15624;  //1Hz
	
	TCCR1B |= (1 << WGM12); // turn on CTC mode
//	TCCR1B |= (1 << CS12) | (1 << CS10); //set prescaler
//	TIMSK1 |= (1 << OCIE1A); //enable interrupt
}

ISR(TIMER1_COMPA_vect) {
	cli();
	static uint16_t count_to_the_second = 0;
	count_to_the_second++;
	if ((count_to_the_second == TIME) && (end_of_time == 0)) {	
		count_to_the_second = 0;
		end_of_time = 1;
	}
	sei();
}
