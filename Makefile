CC=g++
CFLAGS=-ansi -pedantic -Wall -Wextra -O3

all : shadower

shadower : lodepng.cpp shadower.cpp
	$(CC) $(CFLAGS) $^ -o $@


