CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -O2

all: ref_shell

ref_shell: shell.c
	$(CC) $(CFLAGS) -o ref_shell shell.c

clean:
	rm -f ref_shell
