import matplotlib
matplotlib.use('Agg')

import matplotlib.pyplot as plt

sizes = [200, 400, 800, 1200, 1600, 2000]
times = [0.278, 2.287, 20.690, 73.380, 207.872, 505.576]

plt.figure(figsize=(10, 6))

plt.plot(sizes, times, 'o-', linewidth=2, markersize=8, color='blue')

plt.xlabel('Размер матрицы (N)', fontsize=12, fontweight='bold')
plt.ylabel('Время выполнения (сек)', fontsize=12, fontweight='bold')

plt.title('Зависимость времени умножения матриц от их размера', 
          fontsize=14, fontweight='bold', pad=20)

plt.grid(True, alpha=0.3, linestyle='--')

for i, (size, time) in enumerate(zip(sizes, times)):
    plt.annotate(f'{time:.3f}', (size, time), textcoords="offset points", 
                 xytext=(0, 10), ha='center', fontsize=9)

plt.savefig('graph_time_vs_size.png', dpi=300, bbox_inches='tight')