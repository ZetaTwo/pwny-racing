CFLAGS=-s -fstack-protector-all -Wno-incompatible-pointer-types -Wno-int-conversion
SRC=chall21.c
OBJ=$(SRC:.c=.o)
BIN=chall21

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
