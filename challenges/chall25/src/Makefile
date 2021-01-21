CFLAGS=-s -Wall -Werror -z relro
SRC=chall25.c
OBJ=$(SRC:.c=.o)
BIN=chall25
 
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
