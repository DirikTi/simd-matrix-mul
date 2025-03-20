CC = gcc

SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

CFLAGS += -Wno-unused-variable -Wno-unused-function -Wno-deprecated-declarations -g -Wall -Wextra
CFLAGS += -Iinclude

# Matrix Size
MATRIX_SIZE ?= 1024
CFLAGS += -DMATRIX_SIZE=$(MATRIX_SIZE)

# Loglama option
ifeq ($(LOG), 1)
    CFLAGS += -DLOG_ENABLED
endif

ifeq ($(VERIFY), 1)
	CFLAGS += -DVERIFY_ENABLED
endif

UTILS_SRC = $(SRC_DIR)/utils.c
SIMD_SRC = $(SRC_DIR)/simd_matrix.c
NORM_SRC = $(SRC_DIR)/n_matrix.c
VERIFY_SRC = $(SRC_DIR)/verify_matrix.c

SIMD_BIN = $(BIN_DIR)/simd_run
NORM_BIN = $(BIN_DIR)/n_run
VERIFY_BIN = $(BIN_DIR)/verify

$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))

all: simd_run n_run verify

simd_run: $(SIMD_SRC) $(UTILS_SRC)
	$(CC) $(CFLAGS) -O2 $(SIMD_SRC) $(UTILS_SRC) -o $(SIMD_BIN)
	sudo perf stat $(SIMD_BIN)

n_run: $(NORM_SRC) $(UTILS_SRC)
	$(CC) $(CFLAGS) $(NORM_SRC) $(UTILS_SRC) -o $(NORM_BIN)
	sudo perf stat $(NORM_BIN)

verify: $(VERIFY_SRC) $(UTILS_SRC)
	$(CC) $(CFLAGS) $(VERIFY_SRC) $(UTILS_SRC) -o $(VERIFY_BIN)
	$(VERIFY_BIN)

clean:
	rm -rf $(BUILD_DIR)/*
