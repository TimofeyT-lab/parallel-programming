#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <mpi.h>

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    MPI_Init(&argc, &argv);

    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    int n = 2000;
    string fileA = "matrixA.txt";
    string fileB = "matrixB.txt";
    string outFile = "result_mpi.txt";

    vector<vector<double>> A(n, vector<double>(n));
    vector<vector<double>> B(n, vector<double>(n));
    vector<vector<double>> C(n, vector<double>(n));

    if (rank == 0) {
        ifstream fa(fileA), fb(fileB);
        if (!fa || !fb) {
            cerr << "Ошибка открытия файлов\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                fa >> A[i][j];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                fb >> B[i][j];
        fa.close(); fb.close();

        cout << "Параллельное умножение (MPI)" << endl;
        cout << "Процессов: " << num_procs << ", Размер: " << n << "x" << n << endl;
    }
    else {

    }

    MPI_Bcast(A.data(), n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B.data(), n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int rows_per_proc = n / num_procs;
    int start_row = rank * rows_per_proc;
    int end_row = (rank == num_procs - 1) ? n : start_row + rows_per_proc;

    auto start = high_resolution_clock::now();

    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }

    auto end = high_resolution_clock::now();
    double seconds = duration_cast<microseconds>(end - start).count() / 1e6;

    if (rank == 0) {
        for (int proc = 1; proc < num_procs; proc++) {
            int proc_start_row = proc * rows_per_proc;
            int proc_rows = (proc == num_procs - 1) ? n - proc_start_row : rows_per_proc;

            MPI_Recv(C[proc_start_row].data(), proc_rows * n, MPI_DOUBLE,
                proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        ofstream out(outFile);
        if (!out) {
            cerr << "Ошибка записи\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        out << fixed << setprecision(6);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++)
                out << C[i][j] << " ";
            out << "\n";
        }
        out.close();

        cout << "Перемножение выполнено успешно!" << endl;
        cout << "Время: " << seconds << " сек" << endl;
        cout << "Операций: " << (long long)n * n * n << endl;
        cout << "Результат сохранён в: " << outFile << endl;

        ofstream stats("statistics_mpi.txt");
        if (stats) {
            stats << "Размер матриц: " << n << "\n";
            stats << "Количество процессов: " << num_procs << "\n";
            stats << "Время выполнения: " << seconds << "\n";
            stats << "Количество операций: " << (long long)n * n * n << "\n";
            stats.close();
        }
    }
    else {
        int proc_rows = end_row - start_row;
        MPI_Send(C[start_row].data(), proc_rows * n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}