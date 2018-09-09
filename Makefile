CC=g++
CFLAGS=-ansi -pedantic -Wall -Wextra -O3 -fopenmp

all : shadower

shadower : lodepng.cpp shadower.cpp
	$(CC) $(CFLAGS) $^ -o $@


