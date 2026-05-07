#include <mpi.h>
#include <iostream>
#include <vector>
#include <iomanip>
#include <random>

using namespace std;

int main(int argc, char* argv[]) {

    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    const int n = 2000;
    vector<double> A(n * n);
    vector<double> B(n * n);
    vector<double> C(n * n);

    if (rank == 0) {
        mt19937 gen(42); 
        uniform_real_distribution<> dis(-10.0, 10.0); 

        for (int i = 0; i < n * n; i++) {
            A[i] = dis(gen);
            B[i] = dis(gen);
        }

        cout << "Parallel Matrix Multiplication (MPI)" << endl;
        cout << "Processes: " << num_procs << ", Size: " << n << "x" << n << endl;
    }

    double start_time = MPI_Wtime();

    MPI_Bcast(A.data(), n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B.data(), n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int rows_per_proc = n / num_procs;
    int start_row = rank * rows_per_proc;
    int end_row = (rank == num_procs - 1) ? n : start_row + rows_per_proc;

    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }

    if (rank == 0) {
        for (int proc = 1; proc < num_procs; proc++) {
            int proc_start_row = proc * rows_per_proc;
            int proc_rows = (proc == num_procs - 1) ? n - proc_start_row : rows_per_proc;
            
            MPI_Recv(C.data() + proc_start_row * n, proc_rows * n, MPI_DOUBLE,
                     proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        double end_time = MPI_Wtime();
        double total_seconds = end_time - start_time;

        cout << "Multiplication completed successfully!" << endl;
        cout << "Statistics:" << endl;
        cout << "Matrix Size: " << n << "x" << n << endl;
        cout << "Number of Processes: " << num_procs << endl;
        cout << "Execution Time: " << total_seconds << " sec" << endl;
        cout << "Total Operations: " << (long long)n * n * n << endl;
        cout << "Performance: " << ((double)n * n * n / total_seconds) << " ops/sec" << endl;
    } else {
        int proc_rows = end_row - start_row;
        MPI_Send(C.data() + start_row * n, proc_rows * n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}