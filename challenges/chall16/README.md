# Chall 16 (Episode 7e - Speedrun Edition)

## Challenge

Simple buffer overflow with all the mitigations turned on. There are no easy libc
leaks sitting on the stack, however, it is easy to leak stack and pie address as
well as the stack canary by providing different size inputs which will press up
next to values on the stack when they are printed back.

Once all that is done, the player can perform the buffer overflow, but before
`main()` returns `rsi`, `rdi` and `rdx` will be set to have all bits on for the
sport of it.

There is a `win()` function for ease of pwn, but the function signature is:
```c
void win(char *arg2, char *arg1, char *bin)
```

This means that the ROP chain the player uses must set `rdx` and `pop rdx` is a
rare gadget in a standard gcc built ELF. They could of course just call `execve()`
directly in the ROP chain but this would still require them to set `rdx` (since it
was previously set to `0xffffffffffffffff`).

The way they can handle these problems is to either use a universal `rdx` gadget
using `__libc_csu_init()` or just call one of the many functions that can clear
`rdx` for them and then call `execve("/bin/sh", NULL, NULL)` directly in their ROP
chain.

## Protections

```
Arch:     amd64-64-little
RELRO:    Full RELRO
Stack:    Canary found
NX:       NX enabled
PIE:      PIE enabled
```

## Building / Running
```bash
$ cd deploy
$ docker build -t chall16 .
$ docker run -d -p 11540:1337/tcp chall16:latest
```

## Testing Exploit

By default the host and port are set to `challenge.pwny.racing` and `11540`. These can be
overwritten by providing arguments to the `solve.py` script.

```bash
$ cd solve
$ ./solve.py localhost 11540
...

```