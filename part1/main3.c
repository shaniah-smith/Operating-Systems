// main3.c — create two children that sleep/wake randomly; parent waits
// build: gcc -std=c99 main3.c -o my3proc
// run:   ./my3proc

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // fork, getpid, getppid, sleep
#include <sys/types.h>
#include <sys/wait.h>   // wait, WIFEXITED, WEXITSTATUS
#include <time.h>

static void child_process(void) {
    srandom((unsigned)(time(NULL) ^ (getpid() << 16)));

    int rounds = (int)(random() % 30) + 1;     // 1..30 iterations
    int i;
    for (i = 0; i < rounds; i++) {
        pid_t pid  = getpid();
        pid_t ppid = getppid();

        printf("Child Pid: %d is going to sleep!\n", (int)pid);
        fflush(stdout);

        unsigned secs = (unsigned)(random() % 10) + 1;  // 1..10 sec
        sleep(secs);

        printf("Child Pid: %d is awake!\nWhere is my Parent: %d?\n",
               (int)pid, (int)ppid);
        fflush(stdout);
    }
    exit(0);
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);  // cleaner output

    // fork two children
    int i;
    for (i = 0; i < 2; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            while (wait(NULL) > 0) {}
            return 1;
        } else if (pid == 0) {
            child_process();           // child never returns
        }
    }

    // parent waits for both children
    for (i = 0; i < 2; i++) {
        int status = 0;
        pid_t done = wait(&status);
        if (done == -1) { perror("wait"); break; }
        printf("Child Pid: %d has completed\n", (int)done);
    }
    return 0;
}
