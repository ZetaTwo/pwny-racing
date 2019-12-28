# Chall 23 (Episode 11)

## Challenge

This is an AVR challenge, the bug is based on a feature I've seen several
times in UART putchar functions. Whenever a '\n' char is written, there
is also a '\r' char written first. Characters are put in a ring buffer,
but the first increase of the index does not have a modulus.

```
static void uartput(uint8_t c) {
	if(c == (uint8_t) '\n') {
		global.outBuffer[global.outWriteIdx] = (uint8_t) '\r';
		global.outWriteIdx++;
		/* Modulus missing !!! */
	}
	global.outBuffer[global.outWriteIdx] = c;
	global.outWriteIdx++;
	global.outWriteIdx &= 31; /* Modulus present */
}
```

So if a newline is written to the last index,
the byte right after the `outBuffer` is overwritten with '\n'.
This location holds the index to the read buffer. By design ;)

The read buffer is only eight bytes, and there is an equality
check for when it has reached the end. Therefore overwriting
the index with '\n' == 0x0a == 10 will make overflow possible.

However, the read command function zeroes this value before
it starts reading, so it is not possible to use the output
of commands to control the value.

There is a debug function that prints out the progress of
the pwm adjust thread. When the value has reached the
setpoint it prints "reached\n" wich can be used to
overflow the buffer.

## Exploit

The intended solution is:

1. enable debug mode
2. give setpoints until the reply only has a carriage return and no newline
3. overwrite the setpoint value with 0xff
4. wait for the PWM value to reach critical level

