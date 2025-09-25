// signal2.c — every 5 seconds: handler prints, then main prints, forever
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t got_alarm = 0;

static void on_alarm(int sig) {
    (void)sig;
    (void)write(STDOUT_FILENO, "Hello World!\n", 13);  // async-signal-safe
    got_alarm = 1;
    alarm(5);                       // schedule the next alarm
}

static int install_handler(int signum, void (*fn)(int)) {
    struct sigaction sa;
    sa.sa_handler = fn;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;       // keep syscalls going; handler is persistent
    return sigaction(signum, &sa, NULL);
}

int main(void) {
    if (install_handler(SIGALRM, on_alarm) == -1) {
        perror("sigaction");
        return 1;
    }

    got_alarm = 0;
    alarm(5);                       // first alarm in 5s

    for (;;) {
        while (!got_alarm) pause(); // sleep until handler fires
        got_alarm = 0;
        printf("Turing was right!\n");
    }
}
