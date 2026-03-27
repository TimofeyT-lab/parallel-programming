#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <omp.h>

using namespace std;
using namespace std::chrono;

int main() {
    setlocale(LC_ALL, "Russian");

    int n = 2000;
    int numThreads = 1;
    string fileA = "matrixA.txt";
    string fileB = "matrixB.txt";
    string outFile = "result.txt";

    cout << "Размер матрицы: " << n << "x" << n << endl;
    cout << "Количество потоков: " << numThreads << endl;

    vector<vector<double>> A(n, vector<double>(n)),
        B(n, vector<double>(n)),
        C(n, vector<double>(n));

    ifstream fa(fileA);
    if (!fa) {
        cerr << "Ошибка открытия файла " << fileA << "\n";
        return 1;
    }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            fa >> A[i][j];
    fa.close();

    ifstream fb(fileB);
    if (!fb) {
        cerr << "Ошибка открытия файла " << fileB << "\n";
        return 1;
    }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            fb >> B[i][j];
    fb.close();

    omp_set_num_threads(numThreads);

    auto start = high_resolution_clock::now();

#pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++) {
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

    ofstream out(outFile);
    if (!out) {
        cerr << "Ошибка записи\n";
        return 1;
    }
    out << fixed << setprecision(6);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            out << C[i][j] << " ";
        out << "\n";
    }
    out.close();

    cout << endl;
    cout << "Перемножение выполнено успешно!" << endl;
    cout << "Размер: " << n << "x" << n << endl;
    cout << "Потоки: " << numThreads << endl;
    cout << "Время: " << fixed << setprecision(6) << seconds << " сек" << endl;
    cout << "Операций: " << (long long)n * n * n << endl;
    cout << "Результат сохранён в: " << outFile << endl;

    return 0;
}