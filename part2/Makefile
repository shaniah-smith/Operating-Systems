# Makefile for Part 2: Signals Lab
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2

signals: signal1 signal2 timer

signal1: signal1.c
	$(CC) $(CFLAGS) -o signal1 signal1.c

signal2: signal2.c
	$(CC) $(CFLAGS) -o signal2 signal2.c

timer: timer.c
	$(CC) $(CFLAGS) -o timer timer.c

.PHONY: clean
clean:
	rm -f signal1 signal2 timer
