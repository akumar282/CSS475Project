# makefile for building project
#
# make clean - remove binaries
# make test  - test build 

CC=g++
CFLAGS=-Wall -Wextra -g3
CLIBS=-lpqxx -lpq

clean:
	rm -rf bin/*.out

test: clean
	$(CC) $(CFLAGS) src/test.cpp -o bin/test.out $(CLIBS) 
	bin/test.out
