#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <fstream>
#include <iomanip>

struct Body {
    float x, y, z, vx, vy, vz, mass;
};

void init_bodies(std::vector<Body>& bodies, int n) {
    srand(42);
    for (int i = 0; i < n; ++i) {
        bodies[i] = {(float)(rand()%100), (float)(rand()%100), (float)(rand()%100), 0, 0, 0, (float)(rand()%10 + 1)};
    }
}

void save_state(const std::string& filename, const std::vector<Body>& bodies) {
    std::ofstream out(filename);
    for (const auto& b : bodies)
        out << b.x << " " << b.y << " " << b.z << " " << b.vx << " " << b.vy << " " << b.vz << " " << b.mass << "\n";
}

int main() {
    std::vector<int> sizes = {200, 400, 800, 1200, 1600, 2000};
    const float G = 6.674e-11f;
    const float dt = 0.01f;

    std::ofstream report("benchmark_results.csv");
    report << "Size,Threads,Time_sec\n"; // Threads всегда 1 для этой лабы

    for (int n : sizes) {
        std::vector<Body> bodies(n);
        init_bodies(bodies, n);

        if (n == 2000) save_state("initial_state.txt", bodies);

        auto start = std::chrono::high_resolution_clock::now();

        // Расчет взаимодействий
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

        for (int i = 0; i < n; ++i) {
            bodies[i].x += bodies[i].vx * dt;
            bodies[i].y += bodies[i].vy * dt;
            bodies[i].z += bodies[i].vz * dt;
        }

        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - start).count();

        std::cout << "Size: " << n << " | Time: " << duration << "s\n";

        report << n << ",1," << std::fixed << std::setprecision(6) << duration << "\n";

        if (n == 2000) save_state("final_state.txt", bodies);
    }
    report.close();
    return 0;
}