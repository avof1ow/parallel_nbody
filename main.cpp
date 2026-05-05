#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <omp.h> // Заголовочный файл OpenMP

struct Body {
    float x, y, z, vx, vy, vz, mass;
};

void init_bodies(std::vector<Body>& bodies, int n) {
    srand(42);
    for (int i = 0; i < n; ++i) {
        bodies[i] = {(float)(rand()%100), (float)(rand()%100), (float)(rand()%100), 0, 0, 0, (float)(rand()%10 + 1)};
    }
}

int main() {
    std::vector<int> sizes = {200, 400, 800, 1200, 1600, 2000};
    std::vector<int> thread_counts = {1, 2, 4, 8}; // Тестируем разное число потоков
    const float G = 6.674e-11f;
    const float dt = 0.01f;

    std::ofstream report("benchmark_results.csv");
    report << "Size,Threads,Time_sec\n";

    for (int n : sizes) {
        for (int t : thread_counts) {
            std::vector<Body> bodies(n);
            init_bodies(bodies, n);

            omp_set_num_threads(t); // Установка количества потоков

            auto start = std::chrono::high_resolution_clock::now();

            // Основной расчет сил (Parallel Section)
            #pragma omp parallel for schedule(static)
            for (int i = 0; i < n; ++i) {
                float fx = 0, fy = 0, fz = 0;
                for (int j = 0; j < n; ++j) {
                    if (i == j) continue;
                    float dx = bodies[j].x - bodies[i].x;
                    float dy = bodies[j].y - bodies[i].y;
                    float dz = bodies[j].z - bodies[i].z;
                    float distSq = dx*dx + dy*dy + dz*dz + 1e-4f;
                    float invDist3 = 1.0f / (distSq * sqrtf(distSq));
                    float s = bodies[j].mass * invDist3;
                    fx += dx * s; fy += dy * s; fz += dz * s;
                }
                bodies[i].vx += dt * G * fx;
                bodies[i].vy += dt * G * fy;
                bodies[i].vz += dt * G * fz;
            }

            // Обновление позиций
            #pragma omp parallel for
            for (int i = 0; i < n; ++i) {
                bodies[i].x += bodies[i].vx * dt;
                bodies[i].y += bodies[i].vy * dt;
                bodies[i].z += bodies[i].vz * dt;
            }

            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(end - start).count();

            std::cout << "N: " << n << " | Threads: " << t << " | Time: " << duration << "s\n";
            report << n << "," << t << "," << std::fixed << std::setprecision(6) << duration << "\n";
        }
    }
    report.close();
    return 0;
}
