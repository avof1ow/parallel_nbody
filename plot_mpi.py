import os
import numpy as np
import matplotlib.pyplot as plt

def plot_mpi_results():
    # Поиск файла в папках сборки
    paths = ['benchmark_results_mpi.csv', 'cmake-build-debug/benchmark_results_mpi.csv', 'cmake-build-release/benchmark_results_mpi.csv']
    csv_file = next((p for p in paths if os.path.exists(p)), None)

    if not csv_file:
        print("CSV не найден! Сначала запусти mpiexec.")
        return

    data = np.genfromtxt(csv_file, delimiter=',', names=True)
    sizes = np.unique(data['Size'])
    procs = np.unique(data['Processes'])

    if len(procs) < 2:
        print("Мало данных для графиков! Запусти mpiexec с разным числом процессов (1, 2, 4).")
        return

    plt.figure(figsize=(14, 6))

    # Ускорение
    plt.subplot(1, 2, 1)
    for s in sizes:
        subset = data[data['Size'] == s]
        t1 = subset[subset['Processes'] == 1]['Time_sec'][0]
        plt.plot(subset['Processes'], t1 / subset['Time_sec'], 'o-', label=f'N={int(s)}')
    plt.plot(procs, procs, '--', color='gray', label='Ideal')
    plt.title('MPI Speedup')
    plt.legend()
    plt.grid(True)

    # Эффективность
    plt.subplot(1, 2, 2)
    for s in sizes:
        subset = data[data['Size'] == s]
        t1 = subset[subset['Processes'] == 1]['Time_sec'][0]
        eff = (t1 / (subset['Processes'] * subset['Time_sec'])) * 100
        plt.plot(subset['Processes'], eff, 'o-', label=f'N={int(s)}')
    plt.title('MPI Efficiency (%)')
    plt.legend()
    plt.grid(True)

    plt.savefig('mpi_performance.png')
    plt.show()

if __name__ == "__main__":
    plot_mpi_results()