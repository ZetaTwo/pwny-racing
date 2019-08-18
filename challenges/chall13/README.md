# Chall 13 (Episode 7b - Speedrun Edition)

## Challenge

A format string attack with an `exit()` call right after the format string is
printed to the user. Obviously the idea is to overwrite the `exit()` pointer
in the GOT much like `.dtors` writes, however there are many things that can
be after that step, each with benefits and detriments.

The reference exploit is crafted for stability, it will put the program in a
loop by restarting it (pointing `exit()` to `main()`) and then write over
`__malloc_hook` after leaking libc. Another method was tested for speed where
`exit()` could be overwritten with a one shot gadget (after leaking and
restarting). This works because you can control `eax` which is the key
requirement for the gadget that was the most stable.

## Protections

```
Arch:     i386-32-little
RELRO:    Partial RELRO
Stack:    Canary found
NX:       NX enabled
PIE:      No PIE (0x8048000)
```

## Building / Running
```bash
$ cd deploy
$ docker build -t chall13 .
$ docker run -d -p 11537:1337/tcp chall13:latest
```

## Testing Exploit

By default the host and port are set to `challenge.pwny.racing` and `11537`. These can be
overwritten by providing arguments to the `solve.py` script.

```bash
$ cd solve
$ ./solve.py localhost 11537
...
[*] leak: 0xf7d63810
[*] libc: 0xf7cfe000
[+] shell? uid=999(ctf) gid=999(ctf) groups=999(ctf)
[*] Switching to interactive mode
$ ls -al
total 6964
drwxr-xr-x 1 root ctf     4096 Aug 18 17:33 .
drwxr-xr-x 1 root root    4096 Aug 18 17:33 ..
-r-------- 1 root ctf      107 Aug 18 17:19 .config.json
-rwxr-x--- 1 root ctf     9652 Aug 18 17:02 chall
---s--x--x 1 root ctf  7100702 Jul 25 16:28 flag_submitter
-rwxr-x--- 1 root ctf       36 Jul 26 03:41 redir.sh
```