GSL_PATH ?= gsl
CFLAGS = -Iheaders -I$(GSL_PATH)/include -L$(GSL_PATH)/lib  -g -lgsl -lgslcblas
LFLAGS = -fPIC -shared
EFLAGS = -lgsl -lgslcblas
CC = gcc

.PHONY: all build test run_server run_tests install clean

all: build test

build: build/server
	gcc --version

test: build/alltests

run_server: build/server
	LD_LIBRARY_PATH=$(GSL_PATH)/lib ./build/server $(ARGS)

run_tests: build/alltests
	LD_LIBRARY_PATH=$(GSL_PATH)/lib ./build/alltests

install: build/server build/alltests
	cp $^ $(ARGS)

clean:
	find build install -type f -not -name .keep | xargs rm -rf


build/server: build/server.o
	$(CC) $^ -o $@ -ldl $(CFLAGS)

build/alltests: build/tests.o
	$(CC) $^ -o $@ --coverage $(CFLAGS)


build/tests.o: tests/tests.c
	$(CC) -c $< -o $@ --coverage $(CFLAGS)

build/server.o: src/server.c
	$(CC) -c $< -o $@  $(CFLAGS)

build/player.so: src/player.c headers/player.h
	$(CC) $(LFLAGS) $< -o $@ $(CFLAGS)

runGame: build/server build/player.so
	LD_LIBRARY_PATH=$(GSL_PATH)/lib ./build/server ./build/player.so ./build/player.so
