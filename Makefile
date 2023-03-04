# makefile for building project
#
# make clean - remove binaries
# make test  - test build 

CC=g++
CFLAGS=-Wall -Wextra -g3
CLIBS=-lpqxx -lpq

clean:
	rm -rf bin/*.out

test: 
	$(CC) $(CFLAGS) $(CLIBS) src/test.cpp -o bin/test.out
