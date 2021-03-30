GSL_PATH ?= gsl
CFLAGS = --std=c99 -Wall -Wextra -Iheaders -I$(GSL_PATH)/include
LFLAGS = -fPIC -shared
GSLFLAGS = -L$(GSL_PATH)/lib -g -lgsl -lgslcblas
CC = gcc

.PHONY: build test run_server run_tests install clean

build: build/server

test: build/alltests

run-server: build/server
	LD_LIBRARY_PATH=$(GSL_PATH)/lib ./build/server $(ARGS)

run-tests: build/alltests
	LD_LIBRARY_PATH=$(GSL_PATH)/lib ./build/alltests

install: build/server build/alltests build/player.so
	cp $^ install

clean:
	find build install -type f -not -name .keep | xargs rm -rf


build/server: build/server.o build/opt.o build/board.o
	$(CC) $^ -o $@ $(GSLFLAGS) -ldl

build/alltests: build/tests.o build/player_test.o build/dummy.o build/player.o build/board.o build/opt.o
	$(CC) $^ -o $@ --coverage $(GSLFLAGS) -ldl

# TSTOBJ

build/tests.o: tests/tests.c
	$(CC) -c $< -o $@ --coverage $(CFLAGS)

build/player_test.o: tests/player_test.c
	$(CC) -c $< -o $@ --coverage $(CFLAGS)

# OBJ

build/server.o: src/server.c
	$(CC) -c $< -o $@ $(CFLAGS)

build/opt.o: src/opt.c headers/opt.h
	$(CC) -c $< -o $@ $(CFLAGS)

build/board.o: src/board.c headers/board.h
	$(CC) -c $< -o $@ $(CFLAGS)

build/player.o: src/player.c headers/player.h
	$(CC) -c $< -o $@ $(CFLAGS)

build/dummy.o: tests/dummy.c
	$(CC) -c $< -o $@ $(CFLAGS)


# DYNAMIC LIBS

build/player.so: src/player.c src/board.c
	$(CC) $(LFLAGS) $^ -o $@ $(CFLAGS) $(GSLFLAGS)

build/tom.so: src/tom.c src/player.c src/board.c
	$(CC) $(LFLAGS) $^ -o $@ $(CFLAGS) $(GSLFLAGS)

build/jerry.so: src/jerry.c src/player.c src/board.c
	$(CC) $(LFLAGS) $^ -o $@ $(CFLAGS) $(GSLFLAGS)

run-game: build/server build/tom.so build/jerry.so
	LD_LIBRARY_PATH=$(GSL_PATH)/lib ./build/server ./build/tom.so ./build/jerry.so
