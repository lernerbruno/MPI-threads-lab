#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <time.h>
#include "mpi.h"
#define size 16000

int ** getMatrix() {
    // Alocando espaço pra matriz
    int **matrix;
    matrix = malloc(size * sizeof(int *));
    
    if(matrix == NULL) {
        printf("Out of memory\n");
        exit(1);
    }

    for(int i = 0; i < size; i++) {
        matrix[i] = malloc(size * sizeof(int));
        if(matrix[i] == NULL) {
            printf("Out of memory\n");
            exit(1);
        }
    }
    // Gerando a matriz aleatoriamente
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            matrix[i][j] = rand() % 2;
        }
    }
    
    // Lendo a matriz de um arquivo

    // FILE *matrixFile = fopen("matrix.txt","r");
    // if(matrixFile == NULL) {
    //     printf("file could not be opened");
    //     exit(1);
    // }

    // i=0;
    // while(!feof(matrixFile)) {
    //     char temp[size], *token;
    //     fscanf(matrixFile, "%s", temp);
    //     token = strtok(temp, ",");
    //     j=0;
    //     while(token != NULL) {
    //         matrix[i][j] = atoi(token);
    //         token = strtok(NULL, ",");
    //         j++;
    //     }
    //     i++;
    // }
    // fclose(matrixFile)
    return matrix;
}

int * matrixToArray(int **matrix) {
    int *matrixArray;
    for(int i = 0; i < size*size; i++) {
        matrixArray[i] = matrix[i/size][i%size];
    }
    return matrixArray;
}

int ** arrayToMatrix(int **matrixArray) {
    int **matrix;
    for(int i = 0; i < size*size; i++) {
        matrix[i/size][i%size] = matrixArray[i];
    }
    return matrix;
}

int main(int argc, char** argv){
    
    struct timeval startTime, endTime;
    
    MPI_Init(&argc, &argv);

    int localSum = 0, globalSum, myRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    if(myRank == 0) {
        // O objetivo desse nó é pegar a matriz e distribuir pro resto
        int **matrix = getMatrix();
        int *matrixArray = matrixToArray(&matrix);
        int columns = size/2, rows = size/2, rowStart = 0, columnStart = 0;
        printf("Esse é o nó %d calculando um quarto da matriz e a distribuindo", myRank);

        gettimeofday(&startTime, NULL);
        
        MPI_Send(&matrixArray, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        
        for (int i = rowStart; i < rowStart + rows; i++) {
            for (int j = columnStart; j < columnStart + columns; j++) {
                matrix[i][j] *= matrix[i][i];
                localSum += matrix[i][j];
            }
        }
    }
    else if(myRank == 1) {
        matrixArray = (int*) malloc((SIZE/2)*sizeof(int));
        MPI_Recv(&matrixArray, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int **matrix = arrayToMatrix(&matrixArray);
        int columns = size/2, rows = size/2, rowStart = 0, columnStart = size/2;
        printf("Esse é o nó %d calculando um quarto da matriz", myRank);

        int *diagonal = malloc(size * sizeof(int));
        for (int i =0; i < size; i++) {
            diagonal[i] = matrix[i][i];
        }

        for (int i = rowStart; i < rowStart + rows; i++) {
            for (int j = columnStart; j < columnStart + columns; j++) {
                matrix[i][j] *= diagonal[i];
                localSum += matrix[i][j];
            }
        }
    }
    else if(myRank == 2) {
        matrixArray = (int*) malloc((SIZE/2)*sizeof(int));
        MPI_Recv(&matrixArray, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int **matrix = arrayToMatrix(&matrixArray);
        int columns = size/2, rows = size/2, rowStart = size/2, columnStart = 0;
        printf("Esse é o nó %d calculando um quarto da matriz", myRank);

        int *diagonal = malloc(size * sizeof(int));
        for (int i =0; i < size; i++) {
            diagonal[i] = matrix[i][i];
        }

        for (int i = rowStart; i < rowStart + rows; i++) {
            for (int j = columnStart; j < columnStart + columns; j++) {
                matrix[i][j] *= diagonal[i];
                localSum += matrix[i][j];
            }
        }
    }
    else if(myRank == 3) {
        matrixArray = (int*) malloc((SIZE/2)*sizeof(int));
        MPI_Recv(&matrixArray, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int **matrix = arrayToMatrix(&matrixArray);
        int columns = size/2, rows = size/2, rowStart = size/2, columnStart = size/2;
        printf("Esse é o nó %d calculando um quarto da matriz", myRank);

        for (int i = rowStart; i < rowStart + rows; i++) {
            for (int j = columnStart; j < columnStart + columns; j++) {
                matrix[i][j] *= matrix[i][i];
                localSum += matrix[i][j];
            }
        }
    }



    MPI_Reduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0,
             MPI_COMM_WORLD);

    // Print the result
    if (myRank == 0) {
        
        gettimeofday(&endTime, NULL);
        double time = (endTime.tv_sec*1000000 + endTime.tv_usec) - (startTime.tv_sec*1000000 +  startTime.tv_usec);
        
        printf("A soma dos elementos da matriz resultante é = %d\n, tempo é = %d\n", global_sum, time/1000000.0));
    }

    
    MPI_Finalize();
    return 0;
}
 
