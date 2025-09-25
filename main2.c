// main2.c — parent vs child printing (no waiting required for this demo)
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>     // fork

#define MAX_COUNT 200

void ChildProcess(void);
void ParentProcess(void);

int main(void) {
    pid_t pid = fork();
    if (pid == 0) {
        ChildProcess();
    } else if (pid > 0) {
        ParentProcess();
    } else {
        perror("fork");
        return 1;
    }
    return 0;
}

void ChildProcess(void) {
    for (int i = 1; i <= MAX_COUNT; i++)
        printf("   This line is from child, value = %d\n", i);
    printf("   *** Child process is done ***\n");
}

void ParentProcess(void) {
    for (int i = 1; i <= MAX_COUNT; i++)
        printf("This line is from parent, value = %d\n", i);
    printf("*** Parent is done ***\n");
}
