// main.c — simple fork demo (both parent and child run after fork)
// build: gcc -std=c99 main.c -o prog1
// run:   ./prog1

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // fork, getpid, usleep, dprintf
#include <sys/types.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    int i;  // declare outside the for to be extra compatible
    for (i = 1; i <= 20; i++) {
        dprintf(STDOUT_FILENO, "This line is from pid %d, value %d\n",
                (int)getpid(), i);
        usleep(20000); // 20 ms pause so interleaving is easy to see
    }
    return 0;
}
