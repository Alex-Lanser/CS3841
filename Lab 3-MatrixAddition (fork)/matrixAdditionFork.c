#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h> 
#include <signal.h>
#include <sys/mman.h>  
#include <sys/types.h> 

  


#define NS_PER_SEC 1000000000

// Get time in nanoseconds
static inline uint64_t gettime_ns()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * NS_PER_SEC + ts.tv_nsec;
}

int main(int argc, char *argv[])
{
    // Get values in matrix 1
    int rows1, columns1;
    FILE *mat1 = fopen(argv[1], "r");
    fscanf(mat1, "%d", &rows1);
    fscanf(mat1, "%d", &columns1);
    int *matrix1 = malloc(sizeof(int) * rows1 * columns1);
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

    // Print matrix 1
    printf("%s", "Matrix 1:");
    for (int r = 0; r < rows1; r++)
    {
        printf("\n");
        for (int c = 0; c < columns1; c++)
        {
            printf("%i ", matrix1[r * columns1 + c]);
        }
    }
    // Get values in matrix 2
    int rows2, columns2;
    FILE *mat2 = fopen(argv[2], "r");
    fscanf(mat2, "%d", &rows2);
    fscanf(mat2, "%d", &columns2);
    int *matrix2 = malloc(sizeof(int) * rows2 * columns2);
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

    // Print matrix 2
    printf("\n\n%s", "Matrix 2:");
    for (int r = 0; r < rows2; r++)
    {
        printf("\n");
        for (int c = 0; c < columns2; c++)
        {
            printf("%i ", matrix2[r * columns2 + c]);
        }
    }

    // Check to see matrices are same size
    if (rows1 != rows2 && columns1 != columns2)
    {
        printf("%s\n", "Matrices are not of same size");
    }
    else
    {

        /*
        We need the final matrix to be shared memory otherwise each child will be
        modifying a different final matrix so the parent will end with an
        unaltered memory segment. see https://github.com/p-w-rs/CS3841/blob/master/Examples/ipc/ipcshm1.c
        for an example of how to use shared memory
        */
        int MAPPED_SIZE = 128;
        int shmfd = shm_open("/CS3841MEMORY", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (shmfd == -1)
        {
            printf("COULD NOT OPEN SHARED MEMORY SEGMENT\n");
            exit(EXIT_FAILURE);
        }

        // Set the size of the shared memory segment
        ftruncate(shmfd, MAPPED_SIZE);

        int *mapped_space = mmap(NULL, MAPPED_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
        if (mapped_space == MAP_FAILED)
        {
            printf("COULD NOT MMAP\n");
            exit(EXIT_FAILURE);
        }

        uint64_t start = gettime_ns();
        for (int r = 0; r < rows1; r++)
        {
            pid_t pid = fork();
            if (pid < 0){ //error
                perror("fork");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0) // child
            { 
                for (int c = 0; c < columns1; c++)
                {
                    // final matrix needs to be shared memory
                    *mapped_space = matrix1[r * columns1 + c] + matrix2[r * columns1 + c];
                }
                // after addition a child must free all malloced memory
                // also it must call shared memory unmap
                // also it must close the shared memory file descriptor
                //******* you must return after this or the child will not terminate
                //******* instead he will continue and go through the loop
                //******* you can call exit() or just return 0;
                munmap(mapped_space, MAPPED_SIZE);
                close(shmfd);
                exit(0);
            }
            /*
            we don't want the parent to wait here inside the fork loop
            doing so means we only have one child running at a time
            the parent should just immediately go up and fork for another child
            */
            waitpid(pid, NULL, 0);
        }

        /*
        This is where you want to make a loop where the parent will call waitpid(child[r], NULL, 0);
        */ 

        uint64_t end = gettime_ns();

        // Read data from space
        printf("\n\n%s", "Final Matrix:");
        for (int r = 0; r < rows1; r++)
        {
            printf("\n");
            for (int c = 0; c < columns1; c++)
            {
                printf("%i ", mapped_space[r * columns1 + c]);
            }
        }
        printf("\n\n%s%ld%s\n", "Addition took ", end - start, " nanoseconds");
        // after printing the parent must free all malloced memory
        // also it must call shared memory unmap
        // also it must close the shared memory file descriptor
        // it is the last proces so it must also call shm_unlink
        munmap(mapped_space, MAPPED_SIZE);
        close(shmfd);
        shm_unlink("/CS3841MEMORY");
    }
    return 0;
}