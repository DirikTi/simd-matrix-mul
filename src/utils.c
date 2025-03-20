#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <utils.h>

void set_matrix(int32_t A[MATRIX_SIZE][MATRIX_SIZE], int32_t B[MATRIX_SIZE][MATRIX_SIZE], int max_value)
{
    srand(time(NULL));

    for (size_t i = 0; i < MATRIX_SIZE; i++) {
        for (size_t j = 0; j < MATRIX_SIZE; j++) {
            A[i][j] = rand() % max_value;
            B[i][j] = rand() % max_value;
        }
    }
}

void transpose_matrix(int32_t A[MATRIX_SIZE][MATRIX_SIZE], int32_t A_T[MATRIX_SIZE][MATRIX_SIZE], int matrix_size)
{
    for (int i = 0; i < matrix_size; i++) {
        for (int j = 0; j < matrix_size; j++) {
            A_T[j][i] = A[i][j];
        }
    }
}

void log_p_matrix(const char *filename, int32_t *matrix)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Cannot open file log_p_matrix");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            fprintf(file, "%d ", matrix[i * MATRIX_SIZE + j]);  // Tek boyutlu diziden erişim
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void log_matrix(const char *filename, int32_t matrix[MATRIX_SIZE][MATRIX_SIZE]) 
{
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Cannot open file log_matrix");
        return;
    }

    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            fprintf(file, "%d ", matrix[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}