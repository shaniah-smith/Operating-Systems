// shm_processes.c
// Parent & child share 4 ints via SysV shared memory.
// Layout: [0]=a, [1]=b, [2]=sum, [3]=product
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    // 1) Create a private shared segment for 4 integers
    int shm_id = shmget(IPC_PRIVATE, 4 * sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0) { perror("shmget"); return 1; }

    // 2) Attach in parent; child inherits after fork()
    int *ShmPTR = (int *)shmat(shm_id, NULL, 0);
    if (ShmPTR == (void *)-1) { perror("shmat"); return 1; }

    int a, b;
    printf("Enter two integers: ");
    fflush(stdout);
    if (scanf("%d %d", &a, &b) != 2) { fprintf(stderr, "Bad input\n"); return 1; }

    ShmPTR[0] = a;  // input a
    ShmPTR[1] = b;  // input b

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        // ---- Child: compute in shared memory ----
        int x = ShmPTR[0];
        int y = ShmPTR[1];
        ShmPTR[2] = x + y;
        ShmPTR[3] = x * y;
        // optional detach in child
        if (shmdt(ShmPTR) == -1) perror("shmdt(child)");
        _exit(0);
    }

    // ---- Parent: wait, read, print, cleanup ----
    wait(NULL);
    printf("Sum: %d\n",     ShmPTR[2]);
    printf("Product: %d\n", ShmPTR[3]);

    if (shmdt(ShmPTR) == -1) perror("shmdt(parent)");
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) perror("shmctl IPC_RMID");
    return 0;
}
