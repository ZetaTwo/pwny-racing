#include <stdio.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL)))-1)

__attribute__ ((section (".noinit")))
static struct {
	uint8_t PADDING[0x0];
	uint8_t commandState;
	uint8_t dbg;
	uint8_t outReadIdx;
	uint8_t outWriteIdx;
	uint8_t outBuffer[32];
	uint8_t commandLen;
	uint8_t command[8];
	uint8_t monState;
	uint8_t pwmLevel;
	uint8_t pwmState;
	uint16_t adjState;
	uint8_t adjTarget;
} global;

const char STR_RESET[] PROGMEM = "RESET\n";
const char STR_PROMPT[] PROGMEM = "> ";
const char STR_HELP[] PROGMEM = "\nhlp, set x, dbg, rst\n";
const char STR_OK[] PROGMEM = ("\nOK\n");
const char STR_ERR[] PROGMEM = ("\n??? (hlp)\n");
const char STR_INC[] PROGMEM = "+";
const char STR_DEC[] PROGMEM = "-";
const char STR_REACHED[] PROGMEM = "Target reached\n";
const char STR_EMPTY[] PROGMEM = "";
const char STR_CRIT[] PROGMEM = ( "CRIT!!! EM shutdowncode: ");
const char STR_WARN[] PROGMEM = ("WARNING: Almost critical!\n");

static void uartput(uint8_t c) {
		if(c == (uint8_t) '\n') {
			global.outBuffer[global.outWriteIdx] = (uint8_t) '\r';
			global.outWriteIdx++;
		}
		global.outBuffer[global.outWriteIdx] = c;
		global.outWriteIdx++;
		global.outWriteIdx &= 31;
}

static void uartputs(const char *str) {
	/* Note! str must point to PROGMEM. GCC is a shitty AVR compiler */
	uint8_t tmp;
	while((tmp=pgm_read_byte(str)) != '\0') {
		uartput(tmp);
		str++;
	}
}

static void commandInit(void) {
	global.commandState = 0;
}

static void commandTask(void) {
	if (global.commandState == 0) {
		global.commandLen = 0;

		uartputs(STR_PROMPT);

		global.commandState = (uint8_t) 1;
	}
	else {
	/* Is there new UART data? */
#if AVR
		if ((UCSR0A & (1U<<RXC0)) != 0) {
			/* Yes! */
			global.command[global.commandLen] = UDR0;
#else
		tmpReadChar = getchar();
		if(tmpReadChar != -1) {
			global.command[global.commandLen] = tmpReadChar;
#endif
			//uartput((global.command[global.commandLen]>>4) + (uint8_t)'0');
			//uartput((global.command[global.commandLen]&0xf) + (uint8_t)'0');
			if((global.command[global.commandLen] == (uint8_t) '\n') ||
			   (global.command[global.commandLen] == (uint8_t) '\r') ||
			   (global.commandLen == (uint8_t) 7)) {
				/* is 7 or newline : parseCommand */
				global.commandState = (uint8_t) 0;
			}
			else {
				/* != 7: increase and return */
				uartput(global.command[global.commandLen]);
				global.commandLen++;
				return;
			}

		}
		else {
			/* No data, wait for it... */
			return;
		}

		if (global.command[0] == (uint8_t) 'h') {
			uartputs(STR_HELP);
		}
		else if (global.command[0] == (uint8_t) 's' &&
		         global.command[1] == (uint8_t) 'e' &&
		         global.command[2] == (uint8_t) 't') {
			/* set x */
			global.adjTarget = 28 * (global.command[4] - (uint8_t) '0');
			uartputs(STR_OK);
			if(global.dbg != 0) {
				global.dbg = (uint8_t) 2;
			}
		}
		else if (global.command[0] == (uint8_t)'d' &&
		         global.command[1] == (uint8_t)'b' &&
		         global.command[2] == (uint8_t)'g') {
			/* dbg */
			global.dbg = (uint8_t)(0==global.dbg); /* 0 -> 1, {1,2} -> 0 */
			uartputs(STR_OK);
		}
		else if (global.command[0] == (uint8_t) 'r' &&
		         global.command[1] == (uint8_t) 's' &&
		         global.command[2] == (uint8_t) 't') {
			for(;;); /* Wait for WDT */
		}
		else {
			uartputs(STR_ERR);
		}
	}
}

static void pwmInit(void) {
	global.pwmState = 0;
	global.pwmLevel = 0;
	DDRB |= (uint8_t) (1U<<5); // PB5 digital out (LED)
}

static void pwmTask(void) {
	global.pwmState--;

	if(global.pwmState == 0) {
		PORTB &= ~((uint8_t) (1U<<5));
	}
	else if(global.pwmState < global.pwmLevel) {
		PORTB |=  ((uint8_t) (1U<<5));
	}
}

static void adjustInit(void) {
	global.adjState = 0;
	global.adjTarget = (uint8_t) 0xc0;
}

static void adjustTask(void) {
	global.adjState--;
	const char *str;
	if(global.adjState == 0) {
		if(global.adjTarget > global.pwmLevel) {
			global.pwmLevel++;
			str = STR_INC;
		}
		else if(global.adjTarget < global.pwmLevel) {
			global.pwmLevel--;
			str = STR_DEC;
		}
		else if(global.dbg == (uint8_t) 2) {
			str = STR_REACHED;
			global.dbg = (uint8_t) 1;
		}
		else {
			str = STR_EMPTY;
		}
		if(global.dbg != 0) {
			uartputs(str);
		}
		global.adjState = (uint16_t) 0x1337;
	}
}


static void sendInit(void) {
	UCSR0B |= (uint8_t) ((1U << RXEN0) | (1U << TXEN0)); //turn on rx and tx
	UCSR0C = (uint8_t) 6; // 8 databits
	UBRR0H = (uint8_t) (BAUD_PRESCALE >> 8); //load upper 8 bits of baud value into high byte of UBBR
	UBRR0L = (uint8_t) (BAUD_PRESCALE);     //load lower 8 bits of baud value into high byte of UBBR
}

static void sendTask(void) {
	if( global.outReadIdx != global.outWriteIdx) {
		/* There is something to send */
		if ((UCSR0A & (1U << UDRE0)) == 0) {
			/* But we must wait */
		}
		else {
			/* UART is free! */
			UDR0 = global.outBuffer[global.outReadIdx];
			global.outReadIdx = (global.outReadIdx+1) & 31;
		}
	}
}

static void monitorInit(void ) {
	global.monState = 0;
}

static void monitorTask(void) {
	if(global.pwmLevel == (uint8_t) 255) {
		uartputs( STR_CRIT);
		while(global.outReadIdx != global.outWriteIdx) { sendTask(); }

		uint8_t tmp, *i=NULL;
		while((tmp=eeprom_read_byte(i)) != '\0') {
			while ((UCSR0A & (1U << UDRE0)) == 0) {
				/* We must wait */
			}
			UDR0 = tmp;
			i++;
		}
		for(;;);
	}
	else if(global.pwmLevel > (uint8_t) 250) {
		if(global.monState != (uint8_t) 1) {
			uartputs(STR_WARN);
		}
		global.monState = (uint8_t) 1;
	}
	else {
		global.monState = 0;
	}
}

int main(void) {

	uartputs(STR_RESET);
	global.dbg = 0;

	commandInit();
	pwmInit();
	sendInit();
	adjustInit();
	monitorInit();

	wdt_enable((uint8_t) WDTO_500MS);

	for(;;) {
		wdt_reset();
		commandTask();
		pwmTask();
		sendTask();
		adjustTask();
		monitorTask();
	}
}
