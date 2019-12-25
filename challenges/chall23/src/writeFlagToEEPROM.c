#include <stdlib.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL)))-1)

static void sendInit(void) {
	UCSR0B |= (uint8_t) ((1U << RXEN0) | (1U << TXEN0)); //turn on rx and tx
	UCSR0C = (uint8_t) 6; // 8 databits
	UBRR0H = (uint8_t) (BAUD_PRESCALE >> 8); //load upper 8 bits of baud value into high byte of UBBR
	UBRR0L = (uint8_t) (BAUD_PRESCALE);     //load lower 8 bits of baud value into high byte of UBBR
}

static void sendTask(uint8_t c) {
	while ((UCSR0A & (1U << UDRE0)) == 0) {
		/* We must wait */
	}
	UDR0 = c;
}

static void uartputs(const char *str) {
	uint8_t tmp;
	while((tmp=*(str)) != '\0') {
		sendTask(tmp);
		str++;
	}
}

int main(void) {
	sendInit();

	uartputs("Starting\n");

	char flag[] = "flag{https://www.youtube.com/watch?v=dQw4w9WgXcQ}";
	uint16_t i;
	for(i=0; i<=strlen(flag); i++) {
		eeprom_update_byte((uint8_t*)i, flag[i]);
	}

	uartputs("Flag written to EEPROM\n");

	return 0;
}
