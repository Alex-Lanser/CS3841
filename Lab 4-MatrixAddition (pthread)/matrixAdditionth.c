/*
 * Alex Lanser
 * CS3841 - 011
 * Lab 4 - Threaded Matrix Addition
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

#define NS_PER_SEC 1000000000
#define CORE 4

int rows1, columns1;
int rows2, columns2;
int *matrix1;
int *matrix2;
int *finalMatrix;

// Get time in nanoseconds
static inline uint64_t gettime_ns()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * NS_PER_SEC + ts.tv_nsec;
}

// Do the matrix addition for the threads
void *thread_routine()
{
    for (int r = 0; r < rows1; r++)
    {
        for (int c = 0; c < columns1; c++)
        {
            finalMatrix[r * columns1 + c] = matrix1[r * columns1 + c] + matrix2[r * columns1 + c];
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    // Get values in matrix 1
    FILE *mat1 = fopen(argv[1], "r");
    fscanf(mat1, "%d", &rows1);
    fscanf(mat1, "%d", &columns1);
    matrix1 = malloc(sizeof(int) * rows1 * columns1);
    for (int r = 0; r < rows1; r++)
    {
        for (int c = 0; c < columns1; c++)
        {
            int value;
            fscanf(mat1, "%d", &value);
            matrix1[r * columns1 + c] = value;
        }
    }
    fclose(mat1);

    // Get values in matrix 2
    FILE *mat2 = fopen(argv[2], "r");
    fscanf(mat2, "%d", &rows2);
    fscanf(mat2, "%d", &columns2);
    matrix2 = malloc(sizeof(int) * rows2 * columns2);
    for (int r = 0; r < rows2; r++)
    {
        for (int c = 0; c < columns2; c++)
        {
            int value;
            fscanf(mat2, "%d", &value);
            matrix2[r * columns2 + c] = value;
        }
    }
    fclose(mat2);

    // Check to make sure the matrices are the same size
    if (rows1 != rows2 && columns1 != columns2)
    {
        printf("%s\n", "Matrices are not of same size");
    }
    // Matrices are the same size
    else
    {
        uint64_t start = gettime_ns();
        // Store data in space
        finalMatrix = malloc(sizeof(int) * rows1 * columns2);
        pthread_t thread[CORE * 2];
        // Create threads and do the routine
        for (int i = 0; i < CORE * 2; i++)
        {
            if (pthread_create(&thread[i], NULL, thread_routine, NULL) == -1)
            {
                printf("COULD NOT CREATE THREAD");
                exit(EXIT_FAILURE);
            }
        }

        // Wait for all threads to finish
        for (int i = 0; i < CORE * 2; i++)
        {
            pthread_join(thread[i], NULL);
        }
        uint64_t end = gettime_ns();

        // Print final matrix
        printf("\n%s", "Final Matrix:");
        for (int r = 0; r < rows1; r++)
        {
            printf("\n");
            for (int c = 0; c < columns1; c++)
            {
                printf("%i ", finalMatrix[r * columns1 + c]);
            }
        }
        // Print how long addition took
        printf("\n\n%s%ld%s\n", "Addition took ", end - start, " nanoseconds");
        free(finalMatrix);
    }
    return 0;
}
