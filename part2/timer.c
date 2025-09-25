#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

static volatile sig_atomic_t ticks = 0;
static time_t start_time;

void on_alarm(int sig) {
    (void)sig;  // unused
    write(STDOUT_FILENO, "Hello World!\n", 13);
    ticks++;
    alarm(1); // re-arm for next second
}

void on_sigint(int sig) {
    (void)sig;
    time_t end_time = time(NULL);
    printf("\nExiting...\nTotal ticks: %d\nElapsed seconds: %ld\n",
           ticks, (long)(end_time - start_time));
    exit(0);
}

int main(void) {
    start_time = time(NULL);

    if (signal(SIGALRM, on_alarm) == SIG_ERR) {
        perror("signal SIGALRM");
        return 1;
    }
    if (signal(SIGINT, on_sigint) == SIG_ERR) {
        perror("signal SIGINT");
        return 1;
    }

    alarm(1);   // first alarm in 1 second

    for (;;) pause(); // wait for signals
}
