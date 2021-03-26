CFLAGS=-Iheaders -I$(GSL_PATH)/include -L$(GSL_PATH)/lib -lgsl -lcblas -ldl
CC=gcc


.PHONY: all build test install clean

all: build test

build: build/server

test: build/alltests

install: build/server build/alltests
	cp $^ $(ARGS)

clean:
	find build install -type f -not -name .keep | xargs rm -rf


build/server: build/server.o
	$(CC) $(CFLAGS) $^ -o $@

build/alltests: build/tests.o
	$(CC) $(CFLAGS) $^ -o $@


build/tests.o: tests/tests.c
	$(CC) $(CFLAGS) -c $< -o $@

build/server.o: src/server.c headers/player.h
	$(CC) $(CFLAGS) -c $< -o $@

build/player.o: src/player.c headers/player.h
	$(CC) $(CFLAGS) -c $< -o $@
