#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <fcntl.h>
#include <utils.h>

#define CORE_COUNT 4
#define MAX_VALUE 1024

int A[MATRIX_SIZE][MATRIX_SIZE];
int B[MATRIX_SIZE][MATRIX_SIZE];
int C[MATRIX_SIZE][MATRIX_SIZE];

pthread_mutex_t lock;
int cache_index_A_matrix = 0;

void *task(void *arg)
{
    int core_id = *(int*)arg;

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
    
    while (1) {
        pthread_mutex_lock(&lock);
        if (cache_index_A_matrix >= MATRIX_SIZE) {
            pthread_mutex_unlock(&lock);
            return NULL;
        }
        int index_A_matrix = cache_index_A_matrix++;
        pthread_mutex_unlock(&lock);

        int index_B_matrix_row = 0;

        while (index_B_matrix_row < MATRIX_SIZE) {
            int result = 0;
            for (int i = 0; i < MATRIX_SIZE; i++) {
                result += A[index_A_matrix][i] * B[i][index_B_matrix_row];
            }
            C[index_A_matrix][index_B_matrix_row] = result;
            index_B_matrix_row++;
        }
    }
    
    return NULL;
}

int main()
{
    set_matrix(A, B, MAX_VALUE);

    pthread_mutex_init(&lock, NULL);

    int cpu_ids[CORE_COUNT];
    pthread_t threads[CORE_COUNT];

    int cpu_index = 0;
    while (cpu_index < CORE_COUNT) {
        cpu_ids[cpu_index] = cpu_index;
        if (pthread_create(&threads[cpu_index], NULL, task, &cpu_ids[cpu_index]) != 0) {
            perror("Cannot create thread");
            exit(1);
        }
        cpu_index++;
    }
    
    for (int i = 0; i < CORE_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    #if defined(LOG_ENABLED)
        log_matrix("A_log.txt", A);
        log_matrix("B_log.txt", B);
        log_matrix("C_log.txt", C);
    #endif

    #if defined(VERIFY_ENABLED)
        int ret = 0;
        for (int i = 0; i < MATRIX_SIZE; i++) {
            for (int j = 0; j < MATRIX_SIZE; j++) {
                int result = 0;
                for (int k = 0; k < MATRIX_SIZE; k++) {
                    result += A[i][k] * B[k][j];  // **Çarpım kuralı düzeltildi**
                }

                if (result != C[i][j]) {
                    ret = -1;
                    printf("row: %d, column: %d, result: %d, expect: %d \n", i, j, result, C[i][j]);
                }
            }
        }

        if (ret == 0) {
            printf("Success Test\n");
        }
    #endif
    
    return 0;
}
