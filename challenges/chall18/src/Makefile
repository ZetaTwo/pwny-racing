CFLAGS=-static -fstack-protector-all -m32 -O0
SRC=episode8.c
OBJ=$(SRC:.c=.o)
BIN=chall

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
