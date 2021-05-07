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

run-game: build/server build/jerry.so build/geralt.so
	LD_LIBRARY_PATH=$(GSL_PATH)/lib ./build/server ./build/jerry.so ./build/geralt.so -m 8

test: build/alltests
	LD_LIBRARY_PATH=$(GSL_PATH)/lib ./build/alltests

install: build/server build/alltests build/tom.so build/jerry.so build/pablo.so build/pablo_supersaiyan.so
	cp $^ install

clean:
	find build install -type f -not -name .keep | xargs rm -rf

build: build/server build/tom.so build/jerry.so build/pablo.so build/pablo_supersaiyan.so build/geralt.so build/goodboy.so

build/server: build/server.o build/opt.o build/board.o
	$(CC) $^ -o $@ $(LFLAGS)

build/alltests: build/tests.o build/player_test.o build/server_test.o build/crashboy.o build/ia_utils.o build/player.o build/board.o build/opt.o
	$(CC) $^ -o $@ --coverage $(LFLAGS)

# OBJ

build/%.o: src/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

build/%.o: src/ia/%.c
	$(CC) -c -fPIC $< -o $@ $(CFLAGS)

build/player.o: src/player.c
	$(CC) -c -fPIC $< -o $@ $(CFLAGS)

# TSTOBJ

build/%.o: tests/%.c
	$(CC) -c $< -o $@ --coverage $(CFLAGS)

build/crashboy.o: tests/crashboy.c
	$(CC) -c -fPIC $< -o $@ $(CFLAGS)

# DYNAMIC LIBS

build/%.so: build/%.o build/player.o build/board.o build/ia_utils.o
	$(CC) -shared $^ -o $@ $(LFLAGS)
