/*
 * 7) OpenACC program for implementing 2D Jacobi iteration
 *
 * IMPORTANT NOTES
 * - This is a C program - keep the .c extension.
 * - Compile with GPU (NVIDIA HPC SDK):  nvc -acc -Minfo=accel 07_jacobi_2d_openacc.c -o jacobi
 *   Compile with GCC:                   gcc -fopenacc 07_jacobi_2d_openacc.c -o jacobi
 *   Plain compile (no OpenACC):         gcc 07_jacobi_2d_openacc.c -o jacobi
 *     -> the #pragma acc lines are ignored and it runs on the CPU; the output is
 *        the SAME, only no GPU is used.
 * - Run: ./jacobi
 * - No input needed. Grid size N = 10 and ITER = 100 are fixed by #define.
 * - Output is a 10 x 10 matrix: the border stays 100.00 and the inside values
 *   rise towards 100 (heat spreading in from the edges).
 * - -Minfo=accel (nvc only) prints which loops were offloaded to the GPU.
 * - "copy(A)" sends A to the GPU once and brings it back at the end;
 *   "create(B)" makes B only on the GPU. All 100 iterations run on the GPU
 *   without copying data back and forth.
 * - If nvc says "no GPU found" / the program errors on a PC without an NVIDIA
 *   GPU, compile with  nvc -acc=host  or plain gcc instead.
 * - Code checked - no changes needed vs. the PDF. Tested here (CPU build):
 *   first rows of output are 100.00 ... then 100.00 99.96 99.93 99.91 ...
 */
#include <stdio.h>

#define N 10
#define ITER 100

int main() {
    int i, j, k;
    double A[N][N], B[N][N];

    // Initialize matrices
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            A[i][j] = B[i][j] = 0.0;

    // Boundary conditions
    for (i = 0; i < N; i++) {
        A[0][i] = A[N-1][i] = 100.0;
        A[i][0] = A[i][N-1] = 100.0;
    }

    // Jacobi iterations
    #pragma acc data copy(A) create(B)
    {
        for (k = 0; k < ITER; k++) {

            #pragma acc parallel loop collapse(2)
            for (i = 1; i < N-1; i++)
                for (j = 1; j < N-1; j++)
                    B[i][j] = 0.25 * (
                        A[i-1][j] + A[i+1][j] +
                        A[i][j-1] + A[i][j+1]
                    );

            #pragma acc parallel loop collapse(2)
            for (i = 1; i < N-1; i++)
                for (j = 1; j < N-1; j++)
                    A[i][j] = B[i][j];
        }
    }

    printf("Final Matrix:\n");
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++)
            printf("%.2f ", A[i][j]);
        printf("\n");
    }

    return 0;
}
