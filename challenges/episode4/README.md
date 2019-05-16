# Description

The application has a limited format string bug that is limited because:

  1. it is located in the `.data` segment.
  2. although `1024` bytes are read, the string passed to `printf` must be `13` bytes or less.

In addition to those limitations, both `**envp` and `**argv` are cleared as well as there being
no `libc` or user controlled pointers on the stack for the player to use.

## Exploitation

Usually in these limited format string bug challenges, the goal is to forge pointers in an
indirect way (writing a pointer to a pointer, or partially overwriting a `libc` or `ld` pointer).

Once that is achieved the target is usually `__free_hook` or `__malloc_hook` (or one of the more
esoteric options available) which can then be triggered from the same format string. These usual
strategies have gone from novel to formulaic in a few short years as the attack method became
generalised. There has, however, always been another option and that option is what this challenge
is designed to explore.

In a given stack frame the stack pointer (rsp) points to the top of the frame and the base pointer
(rbp) points to the bottom. So as a stack frame is cleaned up, the current base pointer is put into
the stack pointer and the previous base pointer is restored from the stack. This means that base
pointers point to base pointers in a chain. So although it is usually possible to find some function
pointer to subvert, another method is to subvert the stack frame metadata.

In this instance, since the targeted function `output` is `> 2` calls deep, the player can simply
perform their limited format string bug write to one of the saved base pointers and as the functions
return, by the time they return from `main()` they can pivot the stack to somewhere of their choosing.

### Problems

  1. The binary is `PIE` so they don't know where to pivot to.
  2. The write primitive the player can achieve will only let them control a few bytes.

### Solutions

  1. A leak with a format string bug is trivial and the locations of gadgets and buffers can be found.
  2. At the end of each iteration, the player is asked if they want to try again. The limited buffer
     used to store and compare their answer is stored on the stack (but *above* the `output` function
     so not usable to forge pointers). This buffer can be used to make a small trampoline ROP chain in
     order to pivot to the larger buffer in the `.data` segment.

### Strategy

  1. use the fsb to leak addresses of the stack and binary.
  2. use the fsb to write to the lower 2 bytes of the stored base pointer and point it to the `again()`
     buffer that will be used as a trampoline (`pop rbp; ret; leave; ret`).
  3. write two ROP payloads; the trampoline and one in the main buffer to leak libc and read another
     payload over the top of it.
  4. write a payload with libc gadgets that will pop a shell.

Below is a diagram to explain the stack layout:

```
 +---------------------------------+
 |  future again() frame           |
 |                                 |
 |  +---------------------------+  |
 |  | buf[]: 0x5858585858585858 |  | <------  C) the future saved return address ------+
 |  +---------------------------+  |                                                   |
 |  | buf[]: 0x5858585858585858 |  |                                                   |
 |  +---------------------------+  |                                                   |
 |  | buf[]: 0x5858585858585858 |  |                                                   |
 |  +---------------------------+  |                                                   |
 +---------------------------------+                                                   |
 +---------------------------------+                                                   |
 |  output() frame                 |                                                   |
 |                                 |                                                   |
 |  +---------------------------+  |                                                   |
 |  | saved base pointer        |  | ------+                                           |
 |  +---------------------------+  |       |  A) write is performed on this pointer    |
 +---------------------------------+       |                                           |
    +---------------------------+          |                                           |
    | return addr to play()     |          |                                           |
    +---------------------------+          |                                           |
 +---------------------------------+       |                                           |
 |  play() frame                   |       |                                           |
 |                                 |       |                                           |
 |  +---------------------------+  | <-----+                                           |
 |  | saved base pointer        |  | ------+  B) write changes this to point above ----+
 |  +---------------------------+  |       |                                           ^
 +---------------------------------+       |                                           ^
    +---------------------------+          |                                           ^
    | return addr to main()     |          |                                           ^
    +---------------------------+          |                                           ^
 +---------------------------------+       |                                           ^
 |  main() frame                   |       |                                           ^
 |                                 |       |                                           ^
 |  +---------------------------+  | <-----+                                           ^
 |  | saved base pointer        |  |                                                   ^
 |  +---------------------------+  |                                                   ^
 +---------------------------------+          D) pivot to trampoline ROP > > > > > > > ^
    +---------------------------+
    | _exit()                   |
    +---------------------------+
```
