/*
	uart_pty.c

	Copyright 2008, 2009 Michel Pollet <buserror@gmail.com>

 	This file is part of simavr.

	simavr is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	simavr is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with simavr.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sim_network.h"
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include "uart_stdinout.h"
#include "avr_uart.h"
#include "sim_time.h"
#include "sim_hex.h"

DEFINE_FIFO(uint8_t,uart_pty_fifo);

//#define TRACE(_w) _w
#ifndef TRACE
#define TRACE(_w)
#endif

/*
 * called when a byte is send via the uart on the AVR
 */
static void
uart_pty_in_hook(
		struct avr_irq_t * irq,
		uint32_t value,
		void * param)
{
	TRACE(printf("uart_pty_in_hook %02x\n", value);)
	putchar(value);

}


int isThereDataOnStdin(void) {

	fd_set selectset;
	struct timeval timeout = {0,1};
	int retVal;
	FD_ZERO(&selectset);
	FD_SET(0,&selectset);
	return select(1,&selectset,NULL,NULL,&timeout);
}


// try to read bytes. The uart_pty_xoff_hook() will be called when
// other side is full
static void
uart_pty_flush_incoming(
		uart_pty_t * p)
{
	if(p->xon && isThereDataOnStdin()) {
		int byte = getchar();
		if(byte==-1) {exit(1);}

		avr_raise_irq(p->irq + IRQ_UART_PTY_BYTE_OUT, byte);

	}
}

avr_cycle_count_t
uart_pty_flush_timer(
		struct avr_t * avr,
		avr_cycle_count_t when,
		void * param)
{
	uart_pty_t * p = (uart_pty_t*)param;

	uart_pty_flush_incoming(p);
	/* always return a cycle NUMBER not a cycle count */
	return p->xon ? when + avr_hz_to_cycles(p->avr, 1000) : 0;
}

/*
 * Called when the uart has room in it's input buffer. This is called repeateadly
 * if necessary, while the xoff is called only when the uart fifo is FULL
 */
static void
uart_pty_xon_hook(
		struct avr_irq_t * irq,
		uint32_t value,
		void * param)
{
	uart_pty_t * p = (uart_pty_t*)param;
	TRACE(if (!p->xon) printf("uart_pty_xon_hook\n");)
	p->xon = 1;

	uart_pty_flush_incoming(p);

	// if the buffer is not flushed, try to do it later
	if (p->xon)
			avr_cycle_timer_register(p->avr, avr_hz_to_cycles(p->avr, 1000),
						uart_pty_flush_timer, param);
}

/*
 * Called when the uart ran out of room in it's input buffer
 */
static void
uart_pty_xoff_hook(
		struct avr_irq_t * irq,
		uint32_t value,
		void * param)
{
	uart_pty_t * p = (uart_pty_t*)param;
	TRACE(if (p->xon) printf("uart_pty_xoff_hook\n");)
	p->xon = 0;
	avr_cycle_timer_cancel(p->avr, uart_pty_flush_timer, param);
}

static const char * irq_names[IRQ_UART_PTY_COUNT] = {
	[IRQ_UART_PTY_BYTE_IN] = "8<uart_pty.in",
	[IRQ_UART_PTY_BYTE_OUT] = "8>uart_pty.out",
};

void
uart_pty_init(
		struct avr_t * avr,
		uart_pty_t * p)
{
	memset(p, 0, sizeof(*p));

	p->avr = avr;
	p->irq = avr_alloc_irq(&avr->irq_pool, 0, IRQ_UART_PTY_COUNT, irq_names);
	avr_irq_register_notify(p->irq + IRQ_UART_PTY_BYTE_IN, uart_pty_in_hook, p);

}

void
uart_pty_stop(
		uart_pty_t * p)
{
	puts(__func__);
	pthread_kill(p->thread, SIGINT);
	for (int ti = 0; ti < 2; ti++)
		if (p->port[ti].s)
			close(p->port[ti].s);
	void * ret;
	pthread_join(p->thread, &ret);
}

void
uart_pty_connect(
		uart_pty_t * p,
		char uart)
{
	// disable the stdio dump, as we are sending binary there
	uint32_t f = 0;
	avr_ioctl(p->avr, AVR_IOCTL_UART_GET_FLAGS(uart), &f);
	f &= ~AVR_UART_FLAG_STDIO;
	avr_ioctl(p->avr, AVR_IOCTL_UART_SET_FLAGS(uart), &f);

	avr_irq_t * src = avr_io_getirq(p->avr, AVR_IOCTL_UART_GETIRQ(uart), UART_IRQ_OUTPUT);
	avr_irq_t * dst = avr_io_getirq(p->avr, AVR_IOCTL_UART_GETIRQ(uart), UART_IRQ_INPUT);
	avr_irq_t * xon = avr_io_getirq(p->avr, AVR_IOCTL_UART_GETIRQ(uart), UART_IRQ_OUT_XON);
	avr_irq_t * xoff = avr_io_getirq(p->avr, AVR_IOCTL_UART_GETIRQ(uart), UART_IRQ_OUT_XOFF);
	if (src && dst) {
		avr_connect_irq(src, p->irq + IRQ_UART_PTY_BYTE_IN);
		avr_connect_irq(p->irq + IRQ_UART_PTY_BYTE_OUT, dst);
	}
	if (xon)
		avr_irq_register_notify(xon, uart_pty_xon_hook, p);
	if (xoff)
		avr_irq_register_notify(xoff, uart_pty_xoff_hook, p);

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);

}

