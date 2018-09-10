CC=g++
CFLAGS=-ansi -pedantic -Wall -Wextra -O3 -fopenmp -std=c++11

all : shadower

shadower : lodepng.cpp shadower.cpp
	$(CC) $(CFLAGS) $^ -o $@

clean :
	rm -f shadower
