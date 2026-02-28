import numpy as np
import sys

def generate_matrix(size, filename, seed=None):
    """Генерация случайной матрицы и сохранение в файл"""
    if seed is not None:
        np.random.seed(seed)
    
    matrix = np.random.rand(size, size) * 10
    
    with open(filename, 'w', encoding='utf-8') as f:
        for i in range(size):
            for j in range(size):
                f.write(f"{matrix[i][j]:.6f} ")
            f.write("\n")
    
    print(f"Сгенерирована матрица {size}x{size}, сохранена в {filename}")
    return matrix

def main():
    size = 100
    seedA = 42
    seedB = 123
    
    if len(sys.argv) >= 2:
        size = int(sys.argv[1])
  
    generate_matrix(size, "matrixA.txt", seedA)
    generate_matrix(size, "matrixB.txt", seedB)

    print("Матрицы успешно сгенерированы!")

if __name__ == "__main__":
    main()
