/*
 * UART.h
 *
 * Created: 30.11.2021 16:07:53
 *  Author: Admin
 */ 
#ifndef __UART_H___
#define __UART_H___
#include "def.h"
#ifdef UART_DEBUG
#include <avr/io.h>
#include <avr/interrupt.h>

void USART_Init(void);
void DebagUart(const char * data );
void DebagUart_uint8(const char * mess, const int data);
void DebagUart_uint32(const char * mess, const uint32_t data);
void DebagUart_uint32_hex(const char * mess, const uint32_t data);
void DebagUart_uint8_hex(const char * mess, const int data);
void SpeedDebugUart_uint8_hex(const char msg, const char data);
#endif //UART_DEBUG
#endif //__UART_H___