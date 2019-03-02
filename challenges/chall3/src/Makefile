CC=gcc

CFLAGS=-s -z noexecstack -nostdlib -no-pie -fno-pic -Tdata=0x100000000
LFLAGS=
SRC=chall3.S
OBJ=$(SRC:.S=.o)
BIN=chall3

all:
	$(MAKE) $(BIN)
	@/usr/bin/printf '\xcd\x80\xcb' | dd of=$(BIN) bs=1 seek=311 count=3 conv=notrunc 

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LFLAGS)
	echo $(OBJ)

.o:
	$(CC) $(CFLAGS) $< -o $@
	echo ".o"

challenge: all
	rm -f $(OBJ)

clean:
	rm -f $(BIN) $(OBJ)
