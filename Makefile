GSL_PATH ?= gsl
CFLAGS = -Iheaders -I$(GSL_PATH)/include -L$(GSL_PATH)/lib -lgsl -lgslcblas
CC = gcc


.PHONY: all build test run_server run_tests install clean

all: build test

build: build/server

test: build/alltests

run_server: build/server
	LD_LIBRARY_PATH=$(GSL_PATH)/lib ./build/server $(ARGS)

run_tests: build/alltests
	LD_LIBRARY_PATH=$(GSL_PATH)/lib ./build/alltests

install: build/server build/alltests
	cp $^ install

clean:
	find build install -type f -not -name .keep | xargs rm -rf


build/server: build/server.o
	$(CC) $(CFLAGS) $^ -o $@ -ldl

build/alltests: build/tests.o
	$(CC) $(CFLAGS) $^ -o $@ --coverage


build/tests.o: tests/tests.c
	$(CC) $(CFLAGS) -c $< -o $@ --coverage

build/server.o: src/server.c headers/player.h
	$(CC) $(CFLAGS) -c $< -o $@

build/player.o: src/player.c headers/player.h
	$(CC) $(CFLAGS) -c $< -o $@
