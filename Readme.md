# Matrix Multiplication Project

This project implements matrix multiplication using two different algorithms. One uses standard multi-core processing, while the other leverages SIMD (Single Instruction Multiple Data) instructions for higher performance. Additionally, cache optimizations have been applied to improve efficiency. Users can enable or disable logging and configure the matrix size as needed. A verification option is also available to ensure computation correctness.

## Matrix Multiplication

Matrix multiplication involves multiplying elements of two matrices according to specific rules. It is mathematically defined as:

\[
C_{i,j} = \sum_{k=1}^{n} A_{i,k} \times B_{k,j}
\]

Where:
- \( A \) and \( B \) are the matrices to be multiplied.
- \( C \) is the resulting matrix.
- \( i \) and \( j \) are the row and column indices of the output.
- \( k \) is an intermediate variable used in the summation.

## Methods Used

### 1. Standard Matrix Multiplication (Multi-Core Processing)
This method uses traditional loops to perform matrix multiplication. Multi-core processing is utilized to efficiently distribute the workload across CPU cores.

### 2. SIMD-Based Matrix Multiplication
By leveraging SIMD instructions, multiple elements are processed simultaneously, significantly reducing computation time and improving performance.

### 3. Line Cache Optimization
Efficient memory access is crucial in matrix operations. Line cache optimization ensures that sequential data is accessed in memory, making better use of the CPU cache. This reduces memory latency and speeds up computation.

## Features

### 1. Configurable Matrix Size
The matrix size can be set using the `MATRIX_SIZE` parameter. The default size is **1024**, but users can modify it as needed:
```
make simd_run MATRIX_SIZE=4096
```
This command compiles and runs the program with a matrix size of **4096x4096**.

### 2. Verification Option
The `VERIFY` option checks the correctness of the matrix multiplication result. When enabled, the program compares the computed matrix `C` against the expected result:
```
make verify VERIFY=1
```
This command runs the verification process and outputs whether the multiplication was successful or if discrepancies were found.

### 3. Logging Feature
Logging can be enabled using the `LOG=1` parameter. For example:
```
make simd_run LOG=1
```
This command compiles and runs the program with logging enabled. To disable logging, simply run the command without the `LOG=1` parameter.

## Usage

### 1. Run with SIMD Optimization
```
make simd_run
```
This command compiles and runs the matrix multiplication program with SIMD instructions using `-O2` optimization.

### 2. Run Standard Matrix Multiplication
```
make n_run
```
This command compiles and runs the standard matrix multiplication program without SIMD.

### 3. Run Verification Program
```
make verify
```
This command compiles and runs the `verify_matrix.c` program, which checks the correctness of the multiplication operations.

### 4. Clean Compiled Files
```
make clean
```
This command removes compiled files from the `build` directory.

## Conclusion
This project aims to enhance matrix multiplication performance using various optimization techniques. By utilizing SIMD instructions, cache optimizations, and multi-core processing, computations are executed more efficiently. The optional logging feature facilitates debugging and analysis, while the `VERIFY` option ensures that results are accurate. Users can also configure the matrix size to suit their performance and memory requirements.

