GSL_PATH ?= gsl
CFLAGS = --std=c99 -Wall -Wextra -g -Iheaders -I$(GSL_PATH)/include
LFLAGS = -L$(GSL_PATH)/lib -lgsl -lgslcblas -ldl -lm
CC = gcc

.PHONY: build test run_server run_tests install clean

all: build test

# SCRIPTS

run-server: build/server
	LD_LIBRARY_PATH=$(GSL_PATH)/lib ./build/server $(ARGS)

run-tests: build/alltests
	LD_LIBRARY_PATH=$(GSL_PATH)/lib ./build/alltests

run-game: build/server build/pablo.so build/jerry.so
	LD_LIBRARY_PATH=$(GSL_PATH)/lib ./build/server ./build/pablo.so ./build/jerry.so -m 8


install: build/server build/alltests build/tom.so build/jerry.so build/pablo.so
	cp $^ install

clean:
	find build install -type f -not -name .keep | xargs rm -rf

build: build/server build/tom.so build/jerry.so build/pablo.so build/geralt.so

test: build/alltests

build/server: build/server.o build/opt.o build/board.o
	$(CC) $^ -o $@ $(LFLAGS)

build/alltests: build/tests.o build/player_test.o build/dummy.o build/ia_utils.o build/player.o build/board.o build/opt.o
	$(CC) $^ -o $@ --coverage $(LFLAGS)

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

build/ia_utils.o: src/ia_utils.c headers/ia_utils.h
	$(CC) -c $< -o $@ $(CFLAGS)

build/player.o: src/player.c headers/player.h
	$(CC) -c -fPIC $< -o $@ $(CFLAGS)

build/dummy.o: tests/dummy.c
	$(CC) -c -fPIC $< -o $@ $(CFLAGS)

build/tom.o: src/ia/tom.c
	$(CC) -c -fPIC $< -o $@ $(CFLAGS)

build/jerry.o: src/ia/jerry.c
	$(CC) -c -fPIC $< -o $@ $(CFLAGS)

build/pablo.o: src/ia/pablo.c
	$(CC) -c -fPIC $< -o $@ $(CFLAGS)

build/geralt.o: src/ia/geralt.c
	$(CC) -c -fPIC $< -o $@ $(CFLAGS)


# DYNAMIC LIBS

build/tom.so: build/tom.o build/player.o build/board.o build/ia_utils.o
	$(CC) -shared $^ -o $@ $(LFLAGS)

build/jerry.so: build/jerry.o build/player.o build/board.o build/ia_utils.o
	$(CC) -shared $^ -o $@ $(LFLAGS)

build/pablo.so: build/pablo.o build/player.o build/board.o build/ia_utils.o
	$(CC) -shared $^ -o $@ $(LFLAGS)

build/geralt.so: build/geralt.o build/player.o build/board.o build/ia_utils.o
	$(CC) -shared $^ -o $@ $(LFLAGS)
