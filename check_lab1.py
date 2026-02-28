import numpy as np
import sys
import time

def read_matrix_from_file(filename, size):
    """Чтение матрицы из файла"""
    matrix = np.zeros((size, size))
    try:
        with open(filename, 'r', encoding='utf-8') as f:
            for i in range(size):
                row = f.readline().strip().split()
                for j in range(size):
                    matrix[i][j] = float(row[j])
        return matrix
    except Exception as e:
        print(f"Ошибка чтения {filename}: {e}")
        return None

def verify_matrices(result_file, matrixA_file, matrixB_file, size, tolerance=1e-6):
    """Проверка результата умножения матриц"""
    print(f"Размер матрицы: {size}x{size}")
    
    matrixA = read_matrix_from_file(matrixA_file, size)
    matrixB = read_matrix_from_file(matrixB_file, size)
    resultC = read_matrix_from_file(result_file, size)
    
    if matrixA is None or matrixB is None or resultC is None:
        print("Не удалось прочитать одну или несколько матриц")
        return False
    
    print("Матрицы успешно загружены")
    
    start_time = time.time()
    expected = np.matmul(matrixA, matrixB)
    numpy_time = time.time() - start_time
    print(f"Время вычисления : {numpy_time:.6f} секунд")
    print()
    
    difference = np.abs(resultC - expected)
    max_diff = np.max(difference)
    mean_diff = np.mean(difference)
    
    print(f"Максимальная абсолютная разница: {max_diff:.10f}")
    print(f"Средняя абсолютная разница: {mean_diff:.10f}")
    print(f"Допуск: {tolerance}")
    print()
    
    if max_diff < tolerance:
        print("ПРОВЕРКА ПРОЙДЕНА: Результаты совпадают в пределах допуска!")
        return True
    else:
        print("ПРОВЕРКА НЕ ПРОЙДЕНА: Результаты различаются сверх допуска!")
        diff_indices = np.where(difference >= tolerance)
        if len(diff_indices[0]) > 0:
            print("\nПервые несколько различий:")
            for idx in range(min(5, len(diff_indices[0]))):
                i, j = diff_indices[0][idx], diff_indices[1][idx]
                print(f"  Позиция [{i}][{j}]: C++ = {resultC[i][j]:.6f}, "
                      f"NumPy = {expected[i][j]:.6f}, разница = {difference[i][j]:.10f}")
        return False

def main():
    size = 100
    result_file = "result.txt"
    matrixA_file = "matrixA.txt"
    matrixB_file = "matrixB.txt"
    tolerance = 1e-6
    
    if len(sys.argv) >= 2:
        size = int(sys.argv[1])
    if len(sys.argv) >= 3:
        result_file = sys.argv[2]
    if len(sys.argv) >= 4:
        matrixA_file = sys.argv[3]
    if len(sys.argv) >= 5:
        matrixB_file = sys.argv[4]
    if len(sys.argv) >= 6:
        tolerance = float(sys.argv[5])
    
    success = verify_matrices(result_file, matrixA_file, matrixB_file, size, tolerance)
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())