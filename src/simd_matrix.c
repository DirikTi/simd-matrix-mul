#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <fcntl.h>
#include "utils.h"
#include <arm_neon.h>
#include <sys/mman.h>
#include <numa.h>

void cleanup();

#define CORE_COUNT 4
#define PAGE_SIZE getpagesize()

#define MAX_VALUE 64

int32_t A[MATRIX_SIZE][MATRIX_SIZE];
int32_t B[MATRIX_SIZE][MATRIX_SIZE];
int32_t B_T[MATRIX_SIZE][MATRIX_SIZE];

int fd;
int *C;

pthread_mutex_t lock;
int32_t cache_index_A_matrix __attribute__((aligned(64))) = 0;

/*
    C_{i,j} = \sum_{k=1}^{n} A_{i,k} \times B_{k,j}
*/

void *task(void *arg)
{
    int core_id = *(int*)arg;

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
    
    register int index_A_matrix = 0;
    
    while (1) {
        pthread_mutex_lock(&lock);
        if (cache_index_A_matrix >= MATRIX_SIZE) {
            pthread_mutex_unlock(&lock);
            return NULL;
        }

        index_A_matrix = cache_index_A_matrix++;
        pthread_mutex_unlock(&lock);

        int32_t index_B_matrix_row = 0;

        for (int i = 0; i < MATRIX_SIZE; i += 16) {
            __builtin_prefetch(A[index_A_matrix] + i, 0, 3);
        }
        
        int32x4_t result_vec = vdupq_n_s32(0);

        while (index_B_matrix_row < MATRIX_SIZE) {
            result_vec = vdupq_n_s32(0);

            for (int i = 0; i < MATRIX_SIZE; i += 16) {

                int32x4_t a_vec1 = vld1q_s32(A[index_A_matrix] + i);
                int32x4_t a_vec2 = vld1q_s32(A[index_A_matrix] + i + 4);
                int32x4_t a_vec3 = vld1q_s32(A[index_A_matrix] + i + 8);
                int32x4_t a_vec4 = vld1q_s32(A[index_A_matrix] + i + 12);

                int32x4_t b_vec1 = vld1q_s32(B_T[index_B_matrix_row] + i);  
                int32x4_t b_vec2 = vld1q_s32(B_T[index_B_matrix_row] + i + 4);  
                int32x4_t b_vec3 = vld1q_s32(B_T[index_B_matrix_row] + i + 8);  
                int32x4_t b_vec4 = vld1q_s32(B_T[index_B_matrix_row] + i + 12);  

                result_vec = vmlaq_s32(result_vec, a_vec1, b_vec1);
                result_vec = vmlaq_s32(result_vec, a_vec2, b_vec2);
                result_vec = vmlaq_s32(result_vec, a_vec3, b_vec3);
                result_vec = vmlaq_s32(result_vec, a_vec4, b_vec4);
            }

            result_vec = vpaddq_s32(result_vec, result_vec);
            result_vec = vpaddq_s32(result_vec, result_vec);

            *(C + index_A_matrix * MATRIX_SIZE + index_B_matrix_row++) = vgetq_lane_s32(result_vec, 0);
        }
    }
    
    return NULL;
}

void allocate_hugepages()
{
    fd = open("/mnt/hugepages/matrix_C", O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        exit(EXIT_FAILURE);
    }

    ftruncate(fd, MATRIX_SIZE * MATRIX_SIZE * sizeof(int));

    C = mmap(NULL, MATRIX_SIZE * MATRIX_SIZE * sizeof(int),
             PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    

    if (C == MAP_FAILED) {
        printf("Cannot open file");
        exit(EXIT_FAILURE);
    }

    madvise(C, MATRIX_SIZE * MATRIX_SIZE * sizeof(int), MADV_HUGEPAGE);
}

int main()
{
    set_matrix(A, B, MAX_VALUE);

    transpose_matrix(B, B_T, MATRIX_SIZE);

    pthread_mutex_init(&lock, NULL);

    int cpu_ids[CORE_COUNT];
    pthread_t threads[CORE_COUNT];
    
    allocate_hugepages();

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

    #if defined(LOG_ENABLED)
        log_matrix("A_log.txt", A);
        log_matrix("B_log.txt", B);
        log_p_matrix("C_log.txt", C);
    #endif

    #if defined(VERIFY_ENABLED)
        int ret = 0;
        for (int i = 0; i < MATRIX_SIZE; i++) {
            for (int j = 0; j < MATRIX_SIZE; j++) {
                int result = 0;
                for (int k = 0; k < MATRIX_SIZE; k++) {
                    result += A[i][k] * B[k][j];  // **Çarpım kuralı düzeltildi**
                }

                if (result != *(C + i * MATRIX_SIZE + j)) {
                    ret = -1;
                    printf("row: %d, column: %d, result: %d, expect: %d \n", i, j, result, *(C + i * MATRIX_SIZE + j));
                }
            }
        }

        if (ret == 0) {
            printf("Success Test\n");
        }
    #endif

    cleanup();

    return 0;
}

void cleanup()
{
    pthread_mutex_destroy(&lock);
    msync(C, MATRIX_SIZE * MATRIX_SIZE * sizeof(int), MS_SYNC);
    munmap(C, MATRIX_SIZE * MATRIX_SIZE * sizeof(int));
    close(fd);
}