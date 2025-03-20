#ifndef UTILS_H_
#define UTILS_H_

#include <stdint.h>

void set_matrix(int32_t A[MATRIX_SIZE][MATRIX_SIZE], int32_t B[MATRIX_SIZE][MATRIX_SIZE], int max_value);
void transpose_matrix(int32_t A[MATRIX_SIZE][MATRIX_SIZE], int32_t A_T[MATRIX_SIZE][MATRIX_SIZE], int matrix_size);
void log_matrix(const char *filename, int32_t matrix[MATRIX_SIZE][MATRIX_SIZE]);
void log_p_matrix(const char *filename, int32_t *matrix);

#endif // UTILS_H