import os
import numpy as np
import matplotlib.pyplot as plt

def plot_cuda_results():
    # Путь к файлу (проверяем папки сборки)
    paths = ['benchmark_results_cuda.csv', 'cmake-build-debug/benchmark_results_cuda.csv']
    csv_file = next((p for p in paths if os.path.exists(p)), None)

    if not csv_file:
        print("Файл benchmark_results_cuda.csv не найден!")
        return

    data = np.genfromtxt(csv_file, delimiter=',', names=True)

    # Отфильтруем N=200 (эффект прогрева), чтобы график не ломался
    data = data[data['Size'] > 200]

    plt.figure(figsize=(10, 6))
    plt.plot(data['Size'], data['Time_sec'], 's-', color='green', label='CUDA (GPU)')

    plt.title('CUDA Performance: N-Body Simulation')
    plt.xlabel('Number of Bodies (N)')
    plt.ylabel('Time (seconds)')
    plt.yscale('log') # Логарифмическая шкала, так как время очень маленькое
    plt.grid(True, which="both", ls="-", alpha=0.5)
    plt.legend()

    plt.savefig('cuda_performance.png')
    plt.show()

if __name__ == "__main__":
    plot_cuda_results()