// pipes_processes3.c
// Simulate: cat scores | grep <argument> | sort
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <grep-pattern>\n", argv[0]);
        return 1;
    }

    int pipeA[2]; // cat -> grep
    int pipeB[2]; // grep -> sort
    if (pipe(pipeA) == -1 || pipe(pipeB) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid1 = fork();
    if (pid1 < 0) { perror("fork"); return 1; }

    if (pid1 == 0) {
        // -------- P1: cat scores --------
        dup2(pipeA[1], STDOUT_FILENO);
        close(pipeA[0]); close(pipeA[1]);
        close(pipeB[0]); close(pipeB[1]);
        char *cat_args[] = {"cat", "scores", NULL};
        execvp("cat", cat_args);
        perror("execvp cat");
        _exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) { perror("fork"); return 1; }

    if (pid2 == 0) {
        // -------- P2: grep <arg> --------
        dup2(pipeA[0], STDIN_FILENO);
        dup2(pipeB[1], STDOUT_FILENO);
        close(pipeA[0]); close(pipeA[1]);
        close(pipeB[0]); close(pipeB[1]);
        char *grep_args[] = {"grep", argv[1], NULL};
        execvp("grep", grep_args);
        perror("execvp grep");
        _exit(1);
    }

    pid_t pid3 = fork();
    if (pid3 < 0) { perror("fork"); return 1; }

    if (pid3 == 0) {
        // -------- P3: sort --------
        dup2(pipeB[0], STDIN_FILENO);
        close(pipeA[0]); close(pipeA[1]);
        close(pipeB[0]); close(pipeB[1]);
        char *sort_args[] = {"sort", NULL};
        execvp("sort", sort_args);
        perror("execvp sort");
        _exit(1);
    }

    // -------- Parent closes all and waits --------
    close(pipeA[0]); close(pipeA[1]);
    close(pipeB[0]); close(pipeB[1]);
    wait(NULL); wait(NULL); wait(NULL);
    return 0;
}
