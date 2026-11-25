/*
 * 1a) Monte Carlo algorithm for PI calculation - OpenMP
 *
 * IMPORTANT NOTES
 * - Run on LINUX (lab Ubuntu machine / WSL). rand_r() does not exist on
 *   Windows MinGW, so it will not compile there.
 * - Compile: g++ -fopenmp 01a_monte_carlo_pi_openmp.cpp -o pi_omp
 * - Run:     ./pi_omp
 * - Change number of threads (optional): export OMP_NUM_THREADS=4  then ./pi_omp
 * - Input: number of intervals (random points). Larger = more accurate PI.
 *     Sample: 10000000   (output should be close to 3.14159)
 * - Value is random, so PI changes slightly on every run - that is expected.
 * - Keep n between 1 and ~2,000,000,000 (it is an int).
 * - Run it with different thread counts (OMP_NUM_THREADS=1, 2, 4, 8) and
 *   compare the "Time taken" values to show the speed-up.
 * - Fix applied vs. the PDF: added a check for n <= 0 (the PDF version
 *   divides by zero and prints "nan"/"inf" for PI).
 */
#include <iostream>
#include <cstdlib>
#include <omp.h>
#include <ctime>

using namespace std;

int main() {
    int n, count = 0;

    cout << "Enter the number of intervals: ";
    cin >> n;

    if (n <= 0) {
        cout << "Invalid number of intervals!" << endl;
        return 0;
    }

    srand(time(NULL));

    double t = omp_get_wtime();

    #pragma omp parallel
    {
        unsigned int seed = time(NULL) + omp_get_thread_num();

        #pragma omp for reduction(+:count)
        for (int i = 0; i < n; i++) {
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;

            if (x*x + y*y <= 1.0)
                count++;
        }
    }

    t = omp_get_wtime() - t;

    double pi = 4.0 * count / n;

    cout << "Number of threads: " << omp_get_max_threads() << endl;
    cout << "Number of intervals: " << n << endl;
    cout << "Estimated value of PI: " << pi << endl;
    cout << "Time taken: " << t << " seconds" << endl;

    return 0;
}
