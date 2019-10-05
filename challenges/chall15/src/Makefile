CFLAGS=-s -z relro -z now -no-pie -fno-pic -fstack-protector-all -m32
SRC=chall15.c
OBJ=$(SRC:.c=.o)
BIN=chall15

.PHONY : clean

all:
	$(MAKE) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

.o:
	$(CC) $(CFLAGS) $< -o $@

challenge: all
	rm -f $(OBJ)

clean:
	rm -f $(BIN) $(OBJ)
