#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 20          // Matrix size: 20x20
#define NUM_THREADS 10  // Exactly 10 threads, as required

// Global matrices
int matA[MAX][MAX];
int matB[MAX][MAX];

int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX];
int matProductResult[MAX][MAX];

// Data passed to each thread describing its portion of work
typedef struct {
    int thread_id;   // 0 .. NUM_THREADS-1
    int startRow;    // first row this thread is responsible for (inclusive)
    int endRow;      // last row this thread is responsible for (exclusive)
} ThreadData;

// Fill a matrix with random integers in [1, 10]
void fillMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

// Print a matrix in a simple aligned format
void printMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/*
 * Worker thread function.
 * Each thread computes:
 *   - matSumResult = matA + matB
 *   - matDiffResult = matA - matB
 *   - matProductResult = matA * matB (standard matrix multiplication)
 * for the rows in [startRow, endRow).
 */
void* worker(void* args) {
    ThreadData* data = (ThreadData*)args;
    int tid      = data->thread_id;
    int startRow = data->startRow;
    int endRow   = data->endRow;

    // Optional debug (commented out to keep output clean):
    // printf("Thread %d working on rows %d to %d\n", tid, startRow, endRow - 1);

    for (int i = startRow; i < endRow; i++) {
        for (int j = 0; j < MAX; j++) {
            int a = matA[i][j];
            int b = matB[i][j];

            // Sum and difference for this cell
            matSumResult[i][j]  = a + b;
            matDiffResult[i][j] = a - b;

            // Product: inner product of row i of A and column j of B
            int sum = 0;
            for (int k = 0; k < MAX; k++) {
                sum += matA[i][k] * matB[k][j];
            }
            matProductResult[i][j] = sum;
        }
    }

    // Nothing to return
    return NULL;
}

int main(int argc, char* argv[]) {
    srand(time(0));  // Do Not Remove: seeds the random number generator.

    // 1. Fill the input matrices with random values.
    fillMatrix(matA);
    fillMatrix(matB);

    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);

    // 3. Create pthread_t objects and ThreadData for our 10 threads.
    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];

    // Determine how many rows each thread should process.
    // For MAX = 20 and NUM_THREADS = 10, this is exactly 2 rows per thread.
    int rowsPerThread = MAX / NUM_THREADS;

    // 4. Create the threads, each with its own row range.
    for (int t = 0; t < NUM_THREADS; t++) {
        threadData[t].thread_id = t;
        threadData[t].startRow  = t * rowsPerThread;
        // Last thread takes any remaining rows, but in this case it divides evenly.
        threadData[t].endRow    = (t == NUM_THREADS - 1) ?
                                  MAX : threadData[t].startRow + rowsPerThread;

        pthread_create(&threads[t], NULL, worker, &threadData[t]);
    }

    // 5. Wait for all threads to finish before printing results.
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    // 6. Print the resulting matrices.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);

    return 0;
}
