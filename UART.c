/*
 * UART.c
 *
 * Created: 30.11.2021 16:07:16
 *  Author: Admin
 */ 
#define F_CPU 16000000L
#include "def.h"
#ifdef UART_DEBUG
#include <avr/io.h>
#include <avr/interrupt.h>
#include "UART.h"
#include <stdio.h>
#include <string.h>

//#define BAUD_PRESCALER 8 //8MHz 115200 (2)
#define BAUD_PRESCALER 8 //For 115200 baudrate and 16 000 000

#define ASYNCHRONOUS (0<<UMSEL00) // USART Mode Selection

#define DISABLED    (0<<UPM00)
#define EVEN_PARITY (2<<UPM00)
#define ODD_PARITY  (3<<UPM00)
#define PARITY_MODE  DISABLED // USART Parity Bit Selection

#define ONE_BIT (0<<USBS0)
#define TWO_BIT (1<<USBS0)
#define STOP_BIT ONE_BIT      // USART Stop Bit Selection

#define FIVE_BIT  (0<<UCSZ00)
#define SIX_BIT   (1<<UCSZ00)
#define SEVEN_BIT (2<<UCSZ00)
#define EIGHT_BIT (3<<UCSZ00)
#define DATA_BIT   EIGHT_BIT  // USART Data Bit Selection


static volatile unsigned char readyToExchange;
static volatile unsigned char numOfDataToReceive;
static volatile unsigned char *receivedDataPtr;
static volatile unsigned char numOfDataReceived;

//#define SIZE_BUF 128
#define SIZE_BUF 254
//kolcevoj cyklicheskij bufer
static volatile char cycle_buf_tx[SIZE_BUF];
static volatile uint8_t tail_tx  = 0;      //hvost bufera
static volatile uint8_t head_tx  = 0;      //golova
static volatile uint8_t count_tx = 0;     //cymbol counter

void UART_ReceiveData(uint8_t* pReceivedData, uint8_t nNumOfDataToReceive);

void flush_buf (void) {
	tail_tx  = 0;      //hvost bufera
	head_tx  = 0;      //golova
	count_tx = 0;     //cymbol counter	
}

void USART_PutChar(const char sym) {
cli();
	UCSR0B |= (1 << UDRIE0) | (1 << TXEN0);
	if(((UCSR0A & (1<<UDRE0)) != 0) && (count_tx == 0)) {
		UDR0 = sym;
	} else {
		if (count_tx < SIZE_BUF){                    //esli v bufere eshche est' mesto
			cycle_buf_tx[tail_tx] = sym;             //pomeshchaem v nego simvol
			count_tx++;                              //inkrementiruem schetchik simvolov
			tail_tx++;                               //i indeks hvosta bufera
			if (tail_tx == SIZE_BUF) {
				tail_tx = 0;
			}
		}
	}
sei();
}

void DebagUart( const char * data ) {
	char sym;
	while(*data){
		sym = *data++;
		USART_PutChar(sym);
	}
}

void SpeedDebugUart_uint8_hex(const char msg, const char data) {
	char * hex = "0123456789ABCDEF";
	char buf[7];
	buf[0] = msg;
	buf[1] = ' ';
	buf[2] = hex[(data >> 4) &0x0F];
	buf[3] = hex[data&0x0F];
	buf[4] = '\n';
	buf[5] = '\r';
	buf[6] = 0;
	DebagUart(buf);
}

void DebagUart_uint8(const char * mess, const int data) {
	char buf[40];
	sprintf((char*)buf,"%s %d\r\n",mess,data);
	DebagUart(buf);
}

void DebagUart_uint32(const char * mess, const uint32_t data) {
	char buf[40];
	sprintf((char*)buf,"%s %lu\r\n",mess,data);
	DebagUart(buf);
}

void DebagUart_uint32_hex(const char * mess, const uint32_t data) {
	char buf[40];
	sprintf((char*)buf,"%s 0x%08lX\r\n",mess,data);
	DebagUart(buf);
}

void DebagUart_uint8_hex(const char * mess, const int data) {
	char buf[40];
	sprintf((char*)buf,"%s 0x%02X\r\n",mess,data);
	DebagUart(buf);
}

void UART_ReceiveData(uint8_t* pReceivedData, uint8_t nNumOfDataToReceive)
{
	receivedDataPtr = pReceivedData;
	numOfDataToReceive = nNumOfDataToReceive;
	numOfDataReceived = 0;
	readyToExchange = 0;
	UCSR0B |= (1 << RXCIE0) | (1 << RXEN0);
}

ISR(USART_UDRE_vect) {
	if (count_tx > 0){                              //если буфер не пустой
		UDR0 = cycle_buf_tx[head_tx];         //записываем в UDR символ из буфера
		count_tx--;                                      //уменьшаем счетчик символов
		head_tx++;                                 //инкрементируем индекс головы буфера
		if (head_tx == SIZE_BUF) {
			head_tx = 0;
		}
	} else {
		UCSR0B &= ~(1 << UDRIE0);
	}
}

ISR(USART_RX_vect) {
	*receivedDataPtr = UDR0;
	receivedDataPtr++;
	numOfDataReceived++;
	
	if (numOfDataReceived == numOfDataToReceive)
	{
		UCSR0B &= ~((1 << RXCIE0) | (1 << RXEN0));
		readyToExchange = 1;
	}
}

void USART_Init() {
	// Set Baud Rate
//	UCSR0A = (1 << U2X0); //double speed
	UBRR0H = BAUD_PRESCALER >> 8;
	UBRR0L = BAUD_PRESCALER;
	// Set Frame Format
	UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;
	// Enable Receiver and Transmitter
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
}
#endif //UART_DEBUG
