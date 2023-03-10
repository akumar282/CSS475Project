# makefile for building project
#
# make clean - remove binaries
# make test  - test build 

CC=g++
CFLAGS=-Wall -Wextra -g3
CLIBS=-lpqxx -lpq

clean:
	rm -rf bin/*.out

start:
	sudo service postgresql start

stop:
	sudo service postgresql stop

run: shell
	bin/shell.out

new: start
	sudo cp -r ./db /var/lib/postgresql/14/main
	su - postgres
# 	cd to ~/14/main/db
#   run psql
#   \i file necessary

test: clean
	$(CC) $(CFLAGS) src/test.cpp -o bin/test.out $(CLIBS) 

shell: start clean
	$(CC) $(CFLAGS) src/main.cpp src/shell.cpp src/command.cpp src/operation.cpp src/api.cpp -o bin/shell.out $(CLIBS)
	
