/*
 * 1b) Monte Carlo algorithm for PI calculation - MPI
 *
 * IMPORTANT NOTES
 * - Run on LINUX (lab Ubuntu machine / WSL). rand_r() does not exist on
 *   Windows MinGW.
 * - Needs MPI installed:  sudo apt install openmpi-bin libopenmpi-dev
 * - Compile: mpicxx 01b_monte_carlo_pi_mpi.cpp -o pi_mpi
 * - Run:     mpirun -np 4 ./pi_mpi
 *   (-np = number of processes. If you get a "not enough slots" error, add
 *    --oversubscribe:  mpirun --oversubscribe -np 4 ./pi_mpi)
 * - Only process 0 asks for input; type it once.
 *     Sample: 10000000   (output should be close to 3.14159)
 * - Value is random, so PI changes slightly on every run - that is expected.
 * - Run with -np 1, 2, 4 and compare the "Time taken" values to show speed-up.
 * - Do NOT run it as plain ./pi_mpi expecting parallelism - that is only 1 process.
 * - Small fix vs. the PDF: "double t" is initialised to 0 (removes a compiler
 *   warning; the result is unchanged).
 */
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <mpi.h>

using namespace std;

#define ROOT 0

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n;

    if (rank == ROOT) {
        cout << "Enter the number of intervals: ";
        cin >> n;
    }

    MPI_Bcast(&n, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    if (n <= 0) {
        if (rank == ROOT)
            cout << "Invalid number of intervals!" << endl;

        MPI_Finalize();
        return 0;
    }

    double t = 0;

    if (rank == ROOT)
        t = MPI_Wtime();

    int count = 0, total_count = 0;

    unsigned int seed = time(NULL) + rank;

    int local_n = n / size;
    int remainder = n % size;

    if (rank < remainder)
        local_n++;

    for (int i = 0; i < local_n; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;

        if (x*x + y*y <= 1.0)
            count++;
    }

    MPI_Reduce(&count, &total_count, 1, MPI_INT,
               MPI_SUM, ROOT, MPI_COMM_WORLD);

    if (rank == ROOT) {
        t = MPI_Wtime() - t;

        double pi = 4.0 * total_count / n;

        cout << "Number of processes: " << size << endl;
        cout << "Number of intervals: " << n << endl;
        cout << "Estimated value of PI: " << pi << endl;
        cout << "Time taken: " << t << " seconds" << endl;
    }

    MPI_Finalize();
    return 0;
}
