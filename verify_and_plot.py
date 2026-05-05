import numpy as np
import matplotlib.pyplot as plt
import os

def analyze_results():
    # 1. Загрузка данных бенчмарка
    # Формат CSV: Size,Threads,Time_sec
    results_file = 'benchmark_results.csv'
    if not os.path.exists(results_file):
        print(f"Файл {results_file} не найден!")
        return

    data = np.genfromtxt(results_file, delimiter=',', names=True)

    sizes = np.unique(data['Size'])
    threads = np.unique(data['Threads'])

    print("\n" + "="*50)
    print(f"{'Size':<10} | {'Threads':<10} | {'Time (s)':<12} | {'Speedup':<10}")
    print("-" * 50)

    plt.figure(figsize=(12, 5))

    # График ускорения
    plt.subplot(1, 2, 1)

    for size in sizes:
        subset = data[data['Size'] == size]
        t1 = subset[subset['Threads'] == 1]['Time_sec'][0]
        speedup = t1 / subset['Time_sec']

        # Печать таблицы в консоль
        for i in range(len(subset)):
            print(f"{int(size):<10} | {int(subset['Threads'][i]):<10} | {subset['Time_sec'][i]:<12.5f} | {speedup[i]:<10.2f}")

        plt.plot(subset['Threads'], speedup, 'o-', label=f'N={int(size)}')

    plt.plot(threads, threads, '--', color='gray', label='Ideal')
    plt.xlabel('Количество потоков')
    plt.ylabel('Ускорение (S)')
    plt.title('График ускорения (Speedup)')
    plt.legend()
    plt.grid(True)

    # График эффективности
    plt.subplot(1, 2, 2)
    for size in sizes:
        subset = data[data['Size'] == size]
        t1 = subset[subset['Threads'] == 1]['Time_sec'][0]
        efficiency = (t1 / (subset['Threads'] * subset['Time_sec'])) * 100
        plt.plot(subset['Threads'], efficiency, 'o-', label=f'N={int(size)}')

    plt.xlabel('Количество потоков')
    plt.ylabel('Эффективность (E), %')
    plt.title('График эффективности (Efficiency)')
    plt.legend()
    plt.grid(True)

    plt.tight_layout()
    plt.savefig('performance_charts.png')
    print("="*50)
    print("Графики сохранены в файл performance_charts.png")
    plt.show()

if __name__ == "__main__":
    analyze_results()