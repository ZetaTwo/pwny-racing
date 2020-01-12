#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef AVR
#include <avr/eeprom.h>
typedef uint16_t ptr_int_t;

#else /*not AVR, assume X86 */
typedef long ptr_int_t;

#define eeprom_read_byte(x) x86_eeprom_read_byte(x)

#endif /* not AVR */

#if 0
#define debug printf
#else
#define debug(x, ...) ;
#endif

void fail(void) __attribute__((weak, noreturn, alias("x86fail")));

__attribute__((noreturn))
void x86fail(void) {
	printf("Parse fail\n");
	exit(1);
}

char x86_eeprom_read_byte(const uint8_t*p) {
	ptr_int_t i = (ptr_int_t) p;
	char retval = '\0';

	if(i<=4) {
		retval = "Test"[i];
	}

	return retval;
}

#define MAX_STRING_LEN (32)

enum type{
UNKNOWN,
INT,  // 012344
STRING,  // "..."
DICT,  // {
CLOSEDICT,  // }
COLON     ,  // :
LIST  ,  // [
CLOSELIST ,  // ]
COMMA     ,  // ,
};
typedef enum type type_t;

#if 0
void *xmalloc(size_t s) {
	void *ret = malloc(s);
	debug("%p %p\n", ret, &ret);
	if(!ret) {
		printf("Malloc failed!\n");
		exit(1);
	}
	return ret;
}
#else
#define xmalloc(x) malloc(x)
#endif

struct string {
	uint8_t len;
	uint8_t buf[0];
};
typedef struct string string_t;

struct json {
	type_t type;
	union {
		uint16_t val;
		string_t str;
		struct json* ptr;
	}payload[0];
};
typedef struct json json_t;

#define STRING_SIZE(x) (sizeof(type_t) + sizeof(string_t) + x)
#define INT_SIZE    (sizeof(type_t) + 1*sizeof(uint16_t))
#define LIST_SIZE   (sizeof(type_t) + 2*sizeof(json_t*))
#define DICT_SIZE   (sizeof(type_t) + 3*sizeof(json_t*))


static union {
	string_t str;
	int16_t val;
	struct {
		string_t pad1;
		uint8_t pad2[MAX_STRING_LEN];
	} padding;
} token;

enum type getToken(void);
json_t*getObject(void);
json_t*getList(void);
void prettyPrint(json_t*);


uint8_t getHexNibble (void) {
	char c = getchar();
	if(c>='0' && c<='9') {
		return c - '0';
	}
	else if(c>='a' && c<='f') {
		return (c - 'a') + 10;
	}
	else if(c>='A' && c<='F') {
		return (c - 'A') + 10;
	}
	else {
		fail();
	}
}

enum type getToken(void) {
	char c;
	enum type retval = UNKNOWN;
	debug("Getting token\n");

	/* Ignore whitespace */

	do {
		c = getchar();
	} while(((c==' ')  ||
		 (c=='\n') ||
		 (c=='\t') ||
		 (c=='\r')));

	debug("First char of token is: '%c' %x\n", c, c);
	switch(c) {
		case '{':
			retval = DICT;
			break;
		case '}':
			retval = CLOSEDICT;
			break;
		case ':':
			retval = COLON;
			break;
		case '[':
			debug("Got list start\n");
			retval = LIST;
			break;
		case ']':
			debug("Got list end\n");
			retval = CLOSELIST;
			break;
		case ',':
			debug("Got comma\n");
			retval = COMMA;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			debug("Got int:");
			token.val = (uint16_t) (c - '0');
			for(;;) {
				c = getchar();
				if(c<'0' || c>'9') {
					ungetc(c,stdin);
					retval = INT;
					debug(" %u DONE\n",token.val);
					break;
				}
				c -= '0';
				if ((token.val > 6553) || (token.val==6553 && c >= 5)) {
					/* result will not fit in 16 bits */
					fail();
				}
				token.val = (10*token.val) + c;
				debug(" %u",token.val);
			}
			break;

		case '"':
			/* TODO: Handle string */
			debug("Got string:");
			token.str.len = 0;
			for(;;) {
				char store = 'X';
				c = getchar();
				switch(c) {
					case '"':
						/* XXX extra return point here */
						return STRING;
					case '\\':
						c = getchar();
						debug("Escape char type %c\n", c);
						switch(c) {
							case 'x':
								store = getHexNibble();
								store <<=4;
								store |= getHexNibble();
								break;
							case 'n':
								store = '\n';
								break;
							case 'r':
								store = '\r';
								break;
							case 't':
								store = '\t';
								break;
							case '"':
								store = '"';
								break;
							case '\\':
								store = '\\';
								break;
							default:
								fail();
						}
						break;
					default:
						store = c;
				}
				if(token.str.len >= MAX_STRING_LEN) {
					fail();
				}
				token.str.buf[token.str.len++] = store;
			}
		default:
			fail();
	}
	return retval;
}


json_t *getList(void) {
	json_t * retval = (json_t*) xmalloc(LIST_SIZE);
	retval->type = LIST;
	retval->payload[0].ptr = getObject();

	enum type t = getToken();

	switch(t) {
		case COMMA:
			retval->payload[1].ptr = getList();
			break;
		case CLOSELIST:
			retval->payload[1].ptr = NULL;
			break;
		default:
			fail();
	}
	return retval;
}


json_t *getDict(void) {
	enum type t;
	json_t * retval;

	retval  = (json_t*) xmalloc(DICT_SIZE);
	retval->type = DICT;
	debug("Getting key object...");
	retval->payload[0].ptr = getObject();

	t = getToken();
	debug("Got delimeter %u\n", t);
	if(t != COLON) {
		fail();
	}

	debug("Getting value object...\n");
	retval->payload[1].ptr = getObject();

	t = getToken();
	debug("Got delimeter %u\n", t);

	switch(t) {
		case COMMA:
			retval->payload[2].ptr = getDict();
			break;
		case CLOSEDICT:
			retval->payload[2].ptr = NULL;
			break;
		default:
			fail();
	}
	return retval;
}

json_t *getObject(void) {
	debug("Parsing object\n");
	json_t *retval = NULL;
	enum type t = getToken();

	switch(t) {
		case INT:
			retval = (json_t*) xmalloc(INT_SIZE);
			retval->type = INT;
			retval->payload[0].val = token.val;
			break;
		case LIST:
			retval = getList();
			break;
		case STRING:
			retval = (json_t*) xmalloc(STRING_SIZE(token.str.len));
			retval->type = STRING;
			retval->payload[0].str.len = token.str.len;
			memcpy(retval->payload[0].str.buf, token.str.buf, token.str.len);
			break;
		case DICT:
			retval = getDict();
			break;
		default:
			fail();
			retval = NULL;
	}
	return retval;
}

void rputchar(char c) {
	if(c == '\n') {
		putchar('\\');
		putchar('n');
	}
	else if (c == '\r') {
		putchar('\\');
		putchar('r');
	}
	else if (c == '\t') {
		putchar('\\');
		putchar('t');
	}
	else if (c >= 0x7f || c < 0x20) {
		printf("\\x%2.2x", c);
	}
	else {
		if(c == '"' || c == '\\') {
			putchar('\\');
		}
		putchar(c);
	}
}

void prettyPrint(json_t *data) {
	uint8_t i;

	switch(data->type) {
		case INT:
			printf("%u", data->payload[0].val);
			break;
		case LIST:
			printf("[");
			for(;;) {
				prettyPrint(data->payload[0].ptr);
				data = data->payload[1].ptr;
				if(data == NULL)
					break;
				printf(", ");
			}
			printf("]");
			break;
		case DICT:
			printf("{");
			for(;;) {
				prettyPrint(data->payload[0].ptr);
				printf(": ");
				prettyPrint(data->payload[1].ptr);
				data = data->payload[2].ptr;
				if(data == NULL)
					break;
				printf(", ");
			}
			printf("}");
			break;
		case STRING:
			putchar('"');
			for(i=0;i<data->payload[0].str.len;i++) {
				rputchar(data->payload[0].str.buf[i]);
			}
			putchar('"');
			break;
		default:
			printf("{UNKNOWN}");
	}
}

uint8_t check(json_t *data) {
	uint8_t i;
	if(data->type != DICT) {
		return 0;
	}
	if(data->payload[0].ptr->type != INT) {
		return 1;
	}
	if(data->payload[0].ptr->payload[0].val != 1337) {
		return 2;
	}
	if(data->payload[1].ptr->type != STRING) {
		return 3;
	}
	for(i=0;i<data->payload[1].str.len;i++) {
		if(data->payload[1].ptr->payload[0].str.buf[i] != eeprom_read_byte((void*)(ptr_int_t)i)) {
			break;
		}
	}
	return 4+i;
}

int main(void) {
	printf("\n> ");
	json_t *data = getObject();
	prettyPrint(data);
	printf("\n%u\n", check(data));
	return 0;
}