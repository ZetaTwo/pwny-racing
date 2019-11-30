# Chall 21 (Episode 9 - Community Challenge)

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

*Note:* There is an easy way out if the player cannot see the above methods paying
off, they can just overflow, obtain a leak and then restart the program (with `main()`
in their ROP chain for a second run) and do another buffer overflow. This method is
just a little slower than the others (at least the way I do them).

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
$ docker build -t chall21 .
$ docker run -d -p 11540:1337/tcp chall21:latest
```

## Testing Exploit

By default the host and port are set to `challenge.pwny.racing` and `11540`. These can be
overwritten by providing arguments to the `solve.py` script. The exploit will fail sometimes
because of extra null bytes in various leaked addresses or the canary. Just run again if this
happens.

```bash
$ cd solve
$ ./solve.py localhost 11540
...
[*] leak:      0x55e9c8755c1d
[*] base:      0x55e9c8755000
[*] stack:     0x7ffd159d8a10
[*] canary: 0xb1d856e117e4a500
[+] shell? uid=999(ctf) gid=999(ctf) groups=999(ctf)
[*] Switching to interactive mode
$ ls -al
total 6964
drwxr-xr-x 1 root ctf     4096 Aug 21 19:51 .
drwxr-xr-x 1 root root    4096 Aug 18 13:55 ..
-r-------- 1 root ctf      107 Aug 21 19:50 .config.json
-rwxr-x--- 1 root ctf    10224 Aug 21 18:37 chall
---s--x--x 1 root ctf  7100702 Jul 25 16:28 flag_submitter
-rwxr-x--- 1 root ctf       36 Jul 26 03:41 redir.sh

```