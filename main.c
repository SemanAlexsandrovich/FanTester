/*
 * FanTester.c
 *
 * Created: 07.12.2022 14:44:53
 * Author : sshmykov
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "timer.h"
//#include "twi.h"
//#include "lcd.h"

#define TOP 255
#define PWM_PIN 6
#define LED_PIN 5
#define FAN_NUM 5
#define ACCURATE 0
#define SIZEOF_SENDBUF 16
#define ONE_SEC 1000

volatile uint8_t end_of_time = 0;
uint16_t result[FAN_NUM] = {0};
//char line[SIZEOF_SENDBUF];
char first_line[SIZEOF_SENDBUF];
char second_line[SIZEOF_SENDBUF];

void port_setup(void) {
	DDRB = 0x00;//enter mode
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
	/*I2C_Init();
	LCD_ini();
	clearlcd();*/
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
			/*sprintf((char*)first_line, "8:%d  9:%d  10:%d",result[0], result[1], result[2]);
			sprintf((char*)second_line, "  11:%d  12:%d",result[3], result[4]);
			setpos(0,0);
			str_lcd(first_line);
			setpos(0,1);
			str_lcd(second_line);
			*/
			for (uint8_t i = 0; i < FAN_NUM; i++) {
				result[i] = 0;
			}
			end_of_time = 0;
		}
    }
}

