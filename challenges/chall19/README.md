# Chall 19 (Episode 8 Community Challenge)

## Challenge

The player can read or write any file on the system that they have valid
permissions for. There is no specific vulnerability (although there is
some buggy handling of read values, they are not specifically security
bugs).

It is a rather open ended challenge as there are a few ways to do the
challenge (although a file called `.ssh` that has been marked as immutable
to avoid planting of `.ssh` keys just in case the SSH port associated with
the IP is the same as on the docker container).

Ideally they would just acquire leaks from `/proc/self/maps` and write some
injected code/data using `/proc/self/mem`.

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
$ docker build -t chall19 .
$ docker run -d -p 11543:1337/tcp chall19:latest
```

## Testing Exploit

By default the host and port are set to `challenge.pwny.racing` and `11543`. These can be
overwritten by providing arguments to the `solve.py` script.

```bash
$ cd solve
$ ./solve.py localhost 11543
...
[*] base:  0x55adeb4f4000
[*] data:  0x55adeb6f6000
[+] shell? uid=999(ctf) gid=999(ctf) groups=999(ctf)
[*] Switching to interactive mode
$ ls -al
total 6964
drwxr-xr-x 1 root ctf     4096 Aug 20 15:30 .
drwxr-xr-x 1 root root    4096 Aug 18 13:55 ..
-r-------- 1 root ctf      107 Aug 20 15:03 .config.json
-rwxr-x--- 1 root ctf    10224 Aug 20 14:14 chall
---s--x--x 1 root ctf  7100702 Jul 25 16:28 flag_submitter
-rwxr-x--- 1 root ctf       36 Jul 26 03:41 redir.sh
```