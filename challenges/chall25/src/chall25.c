#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <alloca.h>
#include <sys/mman.h>
#include <sys/random.h>

#define NUM_BLOCKS 4
#define NUM_BYTES 4

void banner(){

	char *clear = alloca(0x100);
	memset(clear, '\0', 0x100);

	puts("                               pwny.racing presents...\n\n\e[38;5;2" \
			"07m\e[48;5;15m▄\e[0m\e[38;5;207m█████████▄\e[0m \e[38;5;207m\e[48;5;15m▄" \
			"\e[0m\e[38;5;207m███     \e[38;5;207m\e[48;5;15m▄\e[0m\e[38;5;207m███\e[" \
			"0m \e[38;5;207m\e[48;5;15m▄\e[0m\e[38;5;207m███   \e[38;5;207m\e[48;5;15" \
			"m▄\e[0m\e[38;5;207m███\e[0m \e[38;5;207m▄\e[48;5;15m▄\e[0m\e[38;5;207m██" \
			"██████▄\e[0m \e[38;5;207m\e[48;5;15m▄\e[0m\e[38;5;207m███\e[0m\n\e[38;5;" \
			"207m████\e[48;5;14m▀▀▀████\e[0m \e[38;5;207m████     ████\e[0m \e[38;5;2" \
			"07m█████▄ ████\e[0m \e[38;5;207m\e[48;5;14m████▀▀▀████\e[0m \e[38;5;207m" \
			"████\e[0m\n\e[38;5;207m████▄▄▄████\e[0m \e[38;5;207m████  ▄  ████\e[0m "  \
			"\e[38;5;207m███████████\e[0m \e[38;5;207m████▄▄▄████\e[0m \e[38;5;207m██" \
			"██\e[0m\n\e[38;5;207m██████████\e[48;5;14m▀\e[0m \e[38;5;207m████▄███▄██" \
			"██\e[0m \e[38;5;207m████\e[0m\e[36;1m▀\e[38;5;207m\e[48;5;14m▀█████\e[0m" \
			" \e[38;5;207m\e[48;5;14m▀██████████\e[0m \e[38;5;207m████\e[0m\n\e[38;5;" \
			"207m████\e[0m\e[36;1m▀▀▀▀▀▀\e[0m  \e[38;5;207m\e[48;5;14m██████▀██████\e" \
			"[0m \e[38;5;207m████  \e[0m\e[36;1m▀\e[38;5;207m████\e[0m  \e[36;1m▀▀▀▀▀" \
			"▀\e[0m\e[38;5;207m████\e[0m \e[37;1m\e[48;5;14m▄\e[38;5;207m▄▄▄\e[0m\n\e" \
			"[38;5;207m████\e[0m        \e[38;5;207m████\e[48;5;14m▀\e[0m\e[36;1m▀\e[" \
			"0m \e[36;1m▀\e[0m\e[38;5;207m\e[48;5;14m▀████\e[0m \e[38;5;207m████   ██" \
			"██\e[0m        \e[38;5;207m████\e[0m \e[38;5;207m████\e[0m\n\e[36;1m▀▀▀▀" \
			"\e[0m        \e[36;1m▀▀▀▀     ▀▀▀▀\e[0m \e[36;1m▀▀▀▀   ▀▀▀▀\e[0m        " \
			"\e[36;1m▀▀▀▀\e[0m \e[36;1m▀▀▀▀\e[0m\n");
}

int main(int argc, char *argv[]){
	char *buffer = NULL;
	int ptr = 0;

	setvbuf(stdin, 0, _IONBF, 0);
	setvbuf(stdout, 0, _IONBF, 0);

	alarm(60);
	banner();

	buffer = mmap(NULL, 4096, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

	memset(buffer, 0, 4096);

	printf("Let's get started!\n");

	for (int i = 0; i < NUM_BLOCKS; i++){
		for (int j = 0; j < NUM_BYTES; j++) {
			int c = fgetc(stdin);
			if (c == EOF) {
				exit(EXIT_FAILURE);
			}
			buffer[ptr] = c;
			ptr++;
		}

		if (getrandom(&buffer[ptr], 1, 0) != 1) {
			exit(EXIT_FAILURE);
		}

		ptr++;
	}

	 __asm__(
		"xor %%rax, %%rax\n"
		"xor %%rbx, %%rbx\n"
		"xor %%rcx, %%rcx\n"
		"xor %%rdx, %%rdx\n"
		"xor %%rsi, %%rsi\n"
		"xor %%rdi, %%rdi\n"
		"xor %%r8, %%r8\n"
		"xor %%r9, %%r9\n"
		"xor %%r10, %%r10\n"
		"xor %%r11, %%r11\n"
		"xor %%r12, %%r12\n"
		"xor %%r13, %%r13\n"
		"xor %%r14, %%r14\n"
		"xor %%r15, %%r15\n"
		::: "rax", "rbx", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
	);

	((void(*)())buffer)();

	exit(EXIT_SUCCESS);
}
