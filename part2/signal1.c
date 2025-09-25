// signal1.c — one-shot SIGALRM, then print from main and exit
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t got_alarm = 0;

void on_alarm(int sig) {
    (void)sig;
    printf("Hello World!\n");
    got_alarm = 1; // tell main to continue
}

int main(void) {
    if (signal(SIGALRM, on_alarm) == SIG_ERR) {
        perror("signal");
        return 1;
    }

    alarm(5);                    // deliver SIGALRM in 5 seconds
    while (!got_alarm) pause();  // sleep until we get it

    printf("Turing was right!\n");
    return 0;
}
