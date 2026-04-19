#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <cuda_runtime.h>

using namespace std;
using namespace std::chrono;

__global__ void matrixMultiply(double* A, double* B, double* C, int n) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < n && col < n) {
        double sum = 0.0;
        for (int k = 0; k < n; k++) {
            sum += A[row * n + k] * B[k * n + col];
        }
        C[row * n + col] = sum;
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    int n = 100;
    string fileA = "matrixA.txt";
    string fileB = "matrixB.txt";
    string outFile = "result_cuda.txt";

    vector<double> h_A(n * n), h_B(n * n), h_C(n * n, 0.0);

    ifstream fa(fileA), fb(fileB);
    if (!fa || !fb) {
        cerr << "Ошибка открытия файлов\n"; return 1;
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            fa >> h_A[i * n + j];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            fb >> h_B[i * n + j];
    fa.close(); fb.close();

    double* d_A, * d_B, * d_C;
    cudaMalloc(&d_A, n * n * sizeof(double));
    cudaMalloc(&d_B, n * n * sizeof(double));
    cudaMalloc(&d_C, n * n * sizeof(double));

    cudaMemcpy(d_A, h_A.data(), n * n * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B.data(), n * n * sizeof(double), cudaMemcpyHostToDevice);

    dim3 threadsPerBlock(16, 16);
    dim3 blocksPerGrid((n + threadsPerBlock.x - 1) / threadsPerBlock.x,
        (n + threadsPerBlock.y - 1) / threadsPerBlock.y);

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);

    matrixMultiply << <blocksPerGrid, threadsPerBlock >> > (d_A, d_B, d_C, n);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);

    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);

    cudaMemcpy(h_C.data(), d_C, n * n * sizeof(double), cudaMemcpyDeviceToHost);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    ofstream out(outFile);
    if (!out) {
        cerr << "Ошибка записи\n";
        cudaFree(d_A); cudaFree(d_B); cudaFree(d_C);
        return 1;
    }
    out << fixed << setprecision(6);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            out << h_C[i * n + j] << " ";
        out << "\n";
    }
    out.close();

    cout << "Перемножение выполнено успешно!" << endl;
    cout << "Время выполнения на GPU: " << milliseconds << " мс" << endl;
    cout << "Операций: " << (long long)n * n * n << endl;
    cout << "Результат сохранён в: " << outFile << endl;

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    return 0;
}