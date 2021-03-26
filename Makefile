#this is a makefile

CFLAGS=-Iheaders -lgsl -lcblas
.PHONY: clean all

clean:
	rm quor

all: quor

quor: src/server.c headers/player.h
	gcc $(CFLAGS) $< -o $@
