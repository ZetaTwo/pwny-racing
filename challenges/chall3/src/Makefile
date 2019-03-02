CC=gcc

CFLAGS=-s -z noexecstack -nostdlib -no-pie -fno-pic -Tdata=0x100000000
LFLAGS=
SRC=chall3.S
OBJ=$(SRC:.S=.o)
BIN=chall3

all:
	$(MAKE) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LFLAGS)

.o:
	$(CC) $(CFLAGS) $< -o $@

challenge: all
	rm -f $(OBJ)

clean:
	rm -f $(BIN) $(OBJ)
