CC = g++
CFLAGS = -no-pie -fno-pic -ansi -pedantic -Wall -Wextra -m32 -fstack-protector -Wl,-z,noexecstack

BIN  = chall
OBJ  = Thing.o Redlof.o Elif.o Chall4.o
LIBS =

all: $(BIN) $(OBJ)

# Linking
$(BIN) : $(OBJ)
	echo "compiling main chall"
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(LIBS)

Chall4.o : Chall4.cpp Thing.hpp Redlof.hpp Elif.hpp
	$(CC) $(CFLAGS) -c Chall4.cpp

Thing.o : Thing.cpp Thing.hpp
	$(CC) $(CFLAGS) -c Thing.cpp

Redlof.o : Redlof.cpp Redlof.hpp Thing.hpp Elif.hpp
	$(CC) $(CFLAGS) -c Redlof.cpp

Elif.o : Elif.cpp Elif.hpp Thing.hpp
	$(CC) $(CFLAGS) -c Elif.cpp

# # Compilation of "Folder"
# Folder.o : Folder.cpp Folder.hpp Element.hpp File.hpp
# 	$(ECHO) $(ECHOFLAGS) [+] Compilation of "Folder"...
# 	$(COMP) $(COMPFLAGS) -c Folder.cpp
# 	$(ECHO) $(ECHOFLAGS)

# # Compilation of "File"
# File.o : File.cpp File.hpp Element.hpp
# 	$(ECHO) $(ECHOFLAGS) [+] Compilation of "File"...
# 	$(COMP) $(COMPFLAGS) -c File.cpp
# 	$(ECHO) $(ECHOFLAGS)

# Cleaning rule
# $(CLEAN) :
# 	$(RM) $(RMFLAGS) $(BIN) $(OBJS) core

# .o:
# 	$(CC) $(CFLAGS) $< -o $@

challenge: all
	rm -f $(OBJ)

clean:
	rm -f $(BIN) $(OBJ) core
