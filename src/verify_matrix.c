#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_file(const char *filename, int **matrix) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Dosya açılamadı");
        exit(EXIT_FAILURE);
    }

    char *line_buffer = malloc(MATRIX_SIZE * 6); // Sayılar ve boşluklar için
    if (!line_buffer) {
        perror("Memory allocate error");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MATRIX_SIZE; i++) {
        if (!fgets(line_buffer, MATRIX_SIZE * 6, file)) {
            perror("File read error");
            exit(EXIT_FAILURE);
        }

        char *token = strtok(line_buffer, " ");
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (!token) {
                fprintf(stderr, "File format error\n");
                exit(EXIT_FAILURE);
            }
            matrix[i][j] = atoi(token);
            token = strtok(NULL, " ");
        }
    }

    free(line_buffer);
    fclose(file);
}

int main() {
    // Heap bellekte matrisleri oluştur
    int **A = malloc(MATRIX_SIZE * sizeof(int *));
    int **B = malloc(MATRIX_SIZE * sizeof(int *));
    int **C = malloc(MATRIX_SIZE * sizeof(int *));
    for (int i = 0; i < MATRIX_SIZE; i++) {
        A[i] = malloc(MATRIX_SIZE * sizeof(int));
        B[i] = malloc(MATRIX_SIZE * sizeof(int));
        C[i] = malloc(MATRIX_SIZE * sizeof(int));
    }

    read_file("A_log.txt", A);
    read_file("B_log.txt", B);
    read_file("C_log.txt", C);

    // Matris çarpım doğrulaması
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            int result = 0;
            for (int k = 0; k < MATRIX_SIZE; k++) {
                result += A[i][k] * B[k][j];  // **Çarpım kuralı düzeltildi**
            }

            if (result != C[i][j]) {
                printf("row: %d, column: %d, result: %d, expect: %d \n", i, j, result, C[i][j]);
            }
        }
    }

    // Bellek serbest bırakma
    for (int i = 0; i < MATRIX_SIZE; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }
    free(A);
    free(B);
    free(C);

    return 0;
}
