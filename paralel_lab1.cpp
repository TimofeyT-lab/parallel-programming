#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

int main() {

    setlocale(LC_ALL, "Russian");

    int n = 100;
    string fileA = "matrixA.txt";
    string fileB = "matrixB.txt";
    string outFile = "result.txt";

    vector<vector<double>> A(n, vector<double>(n)), B(n, vector<double>(n)), C(n, vector<double>(n));

    ifstream fa(fileA), fb(fileB);
    if (!fa || !fb) { 
        cerr << "Ошибка открытия файлов\n"; return 1;
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            fa >> A[i][j];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            fb >> B[i][j];
    fa.close(); fb.close();
    
    auto start = high_resolution_clock::now();
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            C[i][j] = 0;
            for (int k = 0; k < n; k++)
                C[i][j] += A[i][k] * B[k][j];
        }
    auto end = high_resolution_clock::now();
    double seconds = duration_cast<microseconds>(end - start).count() / 1e6;

    ofstream out(outFile);
    if (!out) { 
        cerr << "Ошибка записи\n"; return 1; 
    }
    out << fixed << setprecision(6);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            out << C[i][j] << " ";
        out << "\n";
    }
    out.close();

    cout << "Перемножение выполнено успешно!";

    ofstream stats("statistics.txt");
    if (stats) {
        stats << "Размер матриц: " << n << "\nВремя выполнения арифметической операции: " << seconds
            << "\nКоличество операций: " << (long long)n * n * n << "\n";
        stats.close();
    }

    return 0;
}