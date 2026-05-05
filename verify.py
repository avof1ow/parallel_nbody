import numpy as np
import os
import sys

def load_bodies(filename):
    """Загружает данные тел из текстового файла."""
    if not os.path.exists(filename):
        print(f"Ошибка: Файл {filename} не найден.")
        return None
    # Ожидается формат: x y z vx vy vz mass
    try:
        data = np.loadtxt(filename)
        return data
    except Exception as e:
        print(f"Ошибка при чтении {filename}: {e}")
        return None

def calculate_reference(bodies, G=6.674e-11, dt=0.01, softening=1e-4):
    """
    Вычисляет один шаг симуляции на Python (эталон).
    Использует векторизацию NumPy для скорости.
    """
    n = bodies.shape[0]
    pos = bodies[:, 0:3]
    vel = bodies[:, 3:6]
    mass = bodies[:, 6]

    new_vel = vel.copy()

    # Расчет сил (ускорения)
    for i in range(n):
        # Вектор от текущего тела i до всех остальных тел
        diff = pos - pos[i]
        # Квадрат расстояния с коэффициентом смягчения (softening)
        dist_sq = np.sum(diff**2, axis=1) + softening
        inv_dist3 = 1.0 / np.sqrt(dist_sq**3)

        # Ускорение: a = G * m_j * r_ij / |r_ij|^3
        accel = G * (diff.T * (mass * inv_dist3)).T
        new_vel[i] += np.sum(accel, axis=0) * dt

    new_pos = pos + new_vel * dt
    # Возвращаем массив в том же формате
    return np.hstack((new_pos, new_vel, mass.reshape(-1, 1)))

def run_verification():
    print("=== Автоматизированная верификация  ===")

    # 1. Загрузка данных
    initial = load_bodies("initial_state.txt")
    final_cpp = load_bodies("final_state.txt")

    if initial is None or final_cpp is None:
        sys.exit(1)

    print(f"Тел в системе: {len(initial)}")
    print("Вычисление эталона на Python (NumPy)...")

    expected = calculate_reference(initial)


    is_correct = np.allclose(final_cpp[:, 0:3], expected[:, 0:3], rtol=1e-4, atol=1e-5)

    if is_correct:
        print("\n ВЕРИФИКАЦИЯ ПРОЙДЕНА")
        print("Данные C++ совпадают с эталоном NumPy.")
    else:
        max_diff = np.max(np.abs(final_cpp[:, 0:3] - expected[:, 0:3]))
        print("\n ВЕРИФИКАЦИЯ ПРОВАЛЕНА")
        print(f"Максимальное отклонение координат: {max_diff}")
        print("Возможные причины: разный порядок вычислений или недостаточно шагов в Python.")

if __name__ == "__main__":
    run_verification()