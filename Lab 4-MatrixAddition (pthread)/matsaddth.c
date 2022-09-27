#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Value depend on System core
#define CORE 4

typedef struct matrix
{
    int r;
    int c;
    int *v;
} matrix;

matrix A, B, R;

void read_matrix(matrix *m, const char *filename)
{
    FILE *f = fopen(filename, "r");
    fscanf(f, "%d %d", &(m->r), &(m->c));
    m->v = malloc(sizeof(int) * m->r * m->c);
    for (int i = 0; i < m->r; i++)
    {
        for (int j = 0; j < m->c; j++)
        {
            int value;
            fscanf(f, "%d", &value);
            m->v[i * m->c + j] = value;
        }
    }
    fclose(f);
}

void print_matrix(const matrix *m)
{
    for (int i = 0; i < m->r; i++)
    {
        for (int j = 0; j < m->c; j++)
        {
            printf("%d ", m->v[i * m->c + j]);
        }
        printf("\n");
    }
}

typedef struct thargs
{
    int start;
    int end;
} thargs;

// Addition of a Matrix
void *addition(void *arg)
{
    thargs *x = (thargs *)arg;
    // Each thread computes 1/4th of matrix addition
    for (int i = x->start; i < x->end; i++)
    {
        R.v[i] = A.v[i] + B.v[i];
    }
}

int main(int argc, char *argv[])
{

    read_matrix(&A, argv[1]);
    read_matrix(&B, argv[2]);
    if (A.r == B.r && A.c == B.c)
    {
        R.r = A.r;
        R.c = A.c;
        R.v = malloc(sizeof(int) * R.r * R.c);
    }
    printf("Matrix A\n");
    print_matrix(&A);
    printf("\nMatrix B\n");
    print_matrix(&B);

    int comps_per_thread = (CORE * 2) / (A.r * A.c);
    pthread_t thread[CORE * 2];
    thargs args[CORE * 2];
    for (int i = 0; i < CORE * 2; i++)
    {
        thargs args[i] = {i * comps_per_thread, ((i + 1) * comps_per_thread)};
        pthread_create(&thread[i], NULL, &addition, &args[i]);
    }

    // Waiting for join threads after compute
    for (i = 0; i < CORE * 2; i++)
    {

        pthread_join(thread[i], NULL);
    }

    // Display Addition of mat_A and mat_B
    printf("\nSum of Matrix A and B:\n");
    print_matrix(&R);

    return 0;
}