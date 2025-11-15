#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include "process.h"
#include "util.h"

#define DEBUG 0     // change to 1 for verbose output

/**
 * Signature for a function pointer that can compare
 */
typedef int (*Comparer)(const void *a, const void *b);

/**
 * compares 2 processes
 * You can assume: 
 * - Process ids will be unique
 * - No 2 processes will have same arrival time
 */
int my_comparer(const void *this, const void *that)
{
    const Process *p = (const Process *) this;
    const Process *q = (const Process *) that;

    /* Since arrival_time is unique, sort by arrival_time only */
    return p->arrival_time - q->arrival_time;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: ./func-ptr <input-file-path>\n");
        fflush(stdout);
        return 1;
    }

    /*******************/
    /* Parse the input */
    /*******************/
    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid filepath\n");
        fflush(stdout);
        return 1;
    }

    Process *processes = parse_file(input_file);

    /*******************/
    /* sort the input  */
    /*******************/
    Comparer process_comparer = &my_comparer;

#if DEBUG
    int i;
    for (i = 0; i < P_SIZE; i++) {
        printf("%d (%d, %d) ",
               processes[i].pid,
               processes[i].priority,
               processes[i].arrival_time);
    }
    printf("\n");
#endif

    qsort(processes, P_SIZE, sizeof(Process), process_comparer);

    /**************************/
    /* print the sorted data  */
    /**************************/

    {
        int i;
        for (i = 0; i < P_SIZE; i++) {
            printf("%d (%d, %d)\n",
                   processes[i].pid,
                   processes[i].priority,
                   processes[i].arrival_time);
        }
    }

    fflush(stdout);
    fflush(stderr);

    /************/
    /* clean up */
    /************/
    free(processes);
    fclose(input_file);

    return 0;
}
