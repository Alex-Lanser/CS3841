#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define NS_PER_SEC 1000000000

//Get time in nanoseconds
static inline uint64_t gettime_ns()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * NS_PER_SEC + ts.tv_nsec;
}

int main(int argc, char* argv[]){
	//Get values in matrix 1
    int rows1, columns1;
    FILE* mat1 = fopen(argv[1], "r");
    fscanf(mat1, "%d", &rows1);
    fscanf(mat1, "%d", &columns1);
    int* matrix1 = malloc(sizeof(int) * rows1 * columns1);
    for(int r = 0; r < rows1; r++) {
        for(int c = 0; c < columns1; c++) {
        	int value;
            fscanf(mat1, "%d", &value);
            matrix1[r * columns1 + c] = value;
        }
    }
    fclose(mat1);

    //Print matrix 1
    printf("%s", "Matrix 1:");
	for(int r = 0; r < rows1; r++){
	  	printf("\n");
	   	for(int c = 0; c < columns1; c++){
	    	printf("%i ", matrix1[r * columns1 + c]);
	    }
	}
    //Get values in matrix 2
    int rows2, columns2;
    FILE* mat2 = fopen(argv[2], "r");
    fscanf(mat2, "%d", &rows2);
    fscanf(mat2, "%d", &columns2);
    int* matrix2 = malloc(sizeof(int) * rows2 * columns2);
    for(int r = 0; r < rows2; r++) {
        for(int c = 0; c < columns2; c++) {
        	int value;
            fscanf(mat2, "%d", &value);
            matrix2[r * columns2 + c] = value;
        }
    }
    fclose(mat2);

    //Print matrix 2
    printf("\n\n%s", "Matrix 2:");
	for(int r = 0; r < rows2; r++){
	  	printf("\n");
	   	for(int c = 0; c < columns2; c++){
	    	printf("%i ", matrix2[r * columns2 + c]);
	    }
	}

    if(rows1 != rows2){
    	printf("%s\n", "Matrices are not of same size");
    }
    else{
    	uint64_t start = gettime_ns();
	    //Store data in space
	    int* finalMatrix = malloc(sizeof(int) * rows1 * rows2);
	    for(int r = 0; r < rows1; r++){
	    	for(int c = 0; c < columns1; c++){
	    		finalMatrix[r * columns1 + c] = matrix1[r * columns1 + c] + matrix2[r * columns1 + c]; //Add matrices together
	    	}
	    }
	    uint64_t end = gettime_ns();

	    //Read data from space
	    printf("\n\n%s", "Final Matrix:");
	    for(int r = 0; r < rows1; r++){
	    	printf("\n");;
	    	for(int c = 0; c < columns1; c++){
	    		printf("%i ", finalMatrix[r * columns1 + c]);
	    	}
	    }
		printf("\n\n%s%ld%s\n", "Addition took ", end-start, " nanoseconds");
	}
	return 0;
}
