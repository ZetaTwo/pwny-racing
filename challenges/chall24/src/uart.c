#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/wdt.h>

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL)))-1)


void rputchar(char);

int
uart_putchar(char c, FILE *stream)
{
	if (c == '\n')
		uart_putchar('\r', stream);
	while( (UCSR0A & (1 << UDRE0)) == 0) {
		/* spinwait */
		wdt_reset();
	}
	UDR0 = c;
	return c;
}

static char parseBuf[32];
static uint8_t writeIdx = 0;
static uint8_t len = 0;

int
uart_getchar(FILE *stream)
{
	int c;
	while( (((volatile uint8_t) UCSR0A) & (1 << RXC0)) == 0) {
		/* spinwait */
		wdt_reset();
	}
	c = UDR0;
	/* TODO: stash away char */
	parseBuf[writeIdx] = c;
	writeIdx = (writeIdx+1)&31;
	if(len < 32) {
		len++;
	}
	return c;
}

void fail(void) {
	uint8_t readIdx = (writeIdx - len)&31;
	printf("Parse error, got this far:\n");
	for(;len > 0;len--) {
		rputchar(parseBuf[readIdx]);
		readIdx = (readIdx+1)&31;
	}
	printf("\n");
	exit(1);
}

__attribute__((constructor))
void init_uart(void) {
	UCSR0B |= (uint8_t) ((1U << RXEN0) | (1U << TXEN0));
	UCSR0C = (uint8_t) 6; // 8 databits
	UBRR0H = (uint8_t) (BAUD_PRESCALE >> 8);
	UBRR0L = (uint8_t) (BAUD_PRESCALE);
	fdevopen(uart_putchar, uart_getchar);
	wdt_enable((uint8_t) WDTO_500MS);
}
