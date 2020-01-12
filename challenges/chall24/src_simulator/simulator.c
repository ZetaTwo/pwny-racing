/*
	simduino.c

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

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>

#if __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <pthread.h>

#include "sim_avr.h"
#include "avr_ioport.h"
#include "sim_elf.h"
#include "sim_hex.h"
#include "sim_gdb.h"
#include "uart_stdinout.h"
#include "sim_vcd_file.h"

uart_pty_t uart_pty;
avr_t * avr = NULL;
avr_vcd_t vcd_file;

// avr special initalization
void avr_special_init( avr_t * avr, void * data)
{
	printf("%s\n", __func__);
}

// avr special deinitalization
void avr_special_deinit( avr_t* avr, void * data)
{
	printf("%s\n", __func__);
	uart_pty_stop(&uart_pty);
}

int main(int argc, char *argv[])
{
	char boot_path[1024] = "";
	uint32_t boot_base, boot_size;
	char * mmcu = "atmega328p";
	uint32_t freq = 16000000;
	int debug = 0;
	int verbose = 0;

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i] + strlen(argv[i]) - 4, ".hex"))
			strncpy(boot_path, argv[i], sizeof(boot_path));
		else if (!strcmp(argv[i], "-d"))
			debug++;
		else if (!strcmp(argv[i], "-v"))
			verbose++;
		else {
			fprintf(stderr, "%s: invalid argument %s\n", argv[0], argv[i]);
			exit(1);
		}
	}
	if(!strlen(boot_path)) {
		fprintf(stderr, "%s: Error: no hex file given!\n", argv[0]);
		exit(1);
	}

	avr = avr_make_mcu_by_name(mmcu);
	if (!avr) {
		fprintf(stderr, "%s: Error creating the AVR core\n", argv[0]);
		exit(1);
	}

	uint8_t * boot = read_ihex_file(boot_path, &boot_size, &boot_base);
	if (!boot) {
		fprintf(stderr, "%s: Unable to load %s\n", argv[0], boot_path);
		exit(1);
	}

	printf("%s booloader 0x%05x: %d bytes\n", mmcu, boot_base, boot_size);

	avr->custom.init = avr_special_init;
	avr->custom.deinit = avr_special_deinit;
	avr->custom.data = NULL;
	avr_init(avr);
	avr->frequency = freq;

	memcpy(avr->flash + boot_base, boot, boot_size);
	free(boot);
	avr->pc = boot_base;
	/* end of flash, remember we are writing /code/ */
	avr->codeend = avr->flashend;
	avr->log = 1 + verbose;

	// even if not setup at startup, activate gdb if crashing
	avr->gdb_port = 1234;
	if (debug) {
		avr->state = cpu_Stopped;
		avr_gdb_init(avr);
	}

	uart_pty_init(avr, &uart_pty);
	uart_pty_connect(&uart_pty, '0');

	while (1) {
		int state = avr_run(avr);
		if ( state == cpu_Done || state == cpu_Crashed)
			break;
	}

}
