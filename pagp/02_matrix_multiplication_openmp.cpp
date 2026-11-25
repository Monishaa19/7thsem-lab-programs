/*
 * 2) Matrix multiplication - OpenMP
 *
 * IMPORTANT NOTES
 * - Compile: g++ -fopenmp 02_matrix_multiplication_openmp.cpp -o matmul
 * - Run:     ./matmul
 * - Input: matrix size n, then number of threads.
 *     Sample: 4  then  2
 * - A is all 1s and B is all 2s, so EVERY element of the output = 2 * n
 *   (e.g. n = 4 -> every value is 8). Use this to check the answer.
 * - The whole n x n output matrix is printed. For timing comparisons use a big
 *   n (e.g. 500-1000) but the printout will be huge - redirect it:
 *     ./matmul > out.txt
 * - Compare times with 1, 2, 4, 8 threads to show the speed-up.
 * - The commented pragma line shows the version with explicit data scope
 *   (shared(A, B, C, n)); both give the same result.
 * - Only the outer i loop is split between threads; j and k are declared
 *   inside the loops, so they are automatically private to each thread.
 * - Very large n (e.g. 5000+) needs a lot of memory and takes minutes.
 * - Fix applied vs. the PDF: added a check that n and threads are > 0.
 */
#include <iostream>
#include <omp.h>

using namespace std;

int main() {
    int n, th;

    cout << "Enter matrix size: ";
    cin >> n;

    cout << "Enter number of threads: ";
    cin >> th;

    if (n <= 0 || th <= 0) {
        cout << "Invalid input" << endl;
        return 0;
    }

    int *A = new int[n*n];
    int *B = new int[n*n];
    int *C = new int[n*n]();

    for (int i = 0; i < n*n; i++) {
        A[i] = 1;
        B[i] = 2;
    }

    omp_set_num_threads(th);

    double t = omp_get_wtime();

    // Without explicit data scope:
    #pragma omp parallel for
    // With explicit data scope, use:
    // #pragma omp parallel for shared(A, B, C, n)

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                C[n*i+j] += A[n*i+k] * B[n*k+j];

    t = omp_get_wtime() - t;

    cout << "Output matrix:\n";

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            cout << C[n*i+j] << " ";
        cout << endl;
    }

    cout << "Time taken: " << t << " seconds" << endl;

    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}
