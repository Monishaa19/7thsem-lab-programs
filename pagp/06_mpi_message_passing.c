/*
 * 6) MPI Message passing (processes 1..3 send a word to process 0)
 *
 * IMPORTANT NOTES
 * - This is a C program - keep the .c extension and compile with mpicc.
 * - Needs MPI installed:  sudo apt install openmpi-bin libopenmpi-dev
 * - Compile: mpicc 06_mpi_message_passing.c -o msg
 * - Run with EXACTLY 4 processes:
 *     mpirun -np 4 ./msg
 *   Any other -np prints "Run with -np 4" and exits.
 *   If you get a "not enough slots" error:
 *     mpirun --oversubscribe -np 4 ./msg
 * - No input needed. Expected output:
 *     Received HELLO from process 1
 *     Received CSE from process 2
 *     Received RVCE from process 3
 * - Output is always in this order because process 0 receives from
 *   process 1, then 2, then 3 (MPI_Recv names the source explicitly).
 * - Code checked - no changes needed vs. the PDF.
 */
#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define SIZE 4
#define BUFSIZE 32

char *m[SIZE] = {"", "HELLO", "CSE", "RVCE"};

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    char msg[BUFSIZE];

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != SIZE) {
        if (rank == 0)
            printf("Run with -np 4\n");
        MPI_Finalize();
        return 1;
    }

    if (rank != 0) {
        strcpy(msg, m[rank]);
        MPI_Send(msg, BUFSIZE, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }
    else {
        for (int i = 1; i < SIZE; i++) {
            MPI_Recv(msg, BUFSIZE, MPI_CHAR, i, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Received %s from process %d\n", msg, i);
        }
    }

    MPI_Finalize();
    return 0;
}
