#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <iomanip>

struct Body {
    float x, y, z, vx, vy, vz, mass;
};

// Кернел: выполняется на видеокарте
__global__ void compute_forces_kernel(Body* bodies, int n, float G, float dt) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) {
        float fx = 0, fy = 0, fz = 0;
        float soft = 1e-4f;

        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            float dx = bodies[j].x - bodies[i].x;
            float dy = bodies[j].y - bodies[i].y;
            float dz = bodies[j].z - bodies[i].z;
            float distSq = dx*dx + dy*dy + dz*dz + soft;
            float invDist3 = 1.0f / (distSq * sqrtf(distSq));
            float s = bodies[j].mass * invDist3;
            fx += dx * s; fy += dy * s; fz += dz * s;
        }

        // Обновляем физику прямо в кернеле (упрощенно)
        bodies[i].vx += dt * G * fx;
        bodies[i].vy += dt * G * fy;
        bodies[i].vz += dt * G * fz;
        bodies[i].x += bodies[i].vx * dt;
        bodies[i].y += bodies[i].vy * dt;
        bodies[i].z += bodies[i].vz * dt;
    }
}

void init_bodies(std::vector<Body>& bodies, int n) {
    srand(42);
    for (int i = 0; i < n; ++i) {
        bodies[i] = {(float)(rand()%100), (float)(rand()%100), (float)(rand()%100), 0, 0, 0, (float)(rand()%10 + 1)};
    }
}

int main() {
    const float G = 6.674e-11f;
    const float dt = 0.01f;
    std::vector<int> sizes = {200, 400, 800, 1200, 1600, 2000, 5000, 10000}; // На GPU можно брать N побольше!

    std::ofstream report("benchmark_results_cuda.csv");
    report << "Size,Processes,Time_sec\n";

    for (int n : sizes) {
        std::vector<Body> h_bodies(n);
        init_bodies(h_bodies, n);

        Body* d_bodies;
        size_t size = n * sizeof(Body);

        // Выделение памяти на GPU
        cudaMalloc(&d_bodies, size);
        cudaMemcpy(d_bodies, h_bodies.data(), size, cudaMemcpyHostToDevice);

        // Настройка сетки (блоки по 256 потоков)
        int threadsPerBlock = 256;
        int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;

        // Замер времени CUDA событиями
        cudaEvent_t start, stop;
        cudaEventCreate(&start);
        cudaEventCreate(&stop);

        cudaEventRecord(start);
        compute_forces_kernel<<<blocksPerGrid, threadsPerBlock>>>(d_bodies, n, G, dt);
        cudaEventRecord(stop);

        cudaEventSynchronize(stop);
        float milliseconds = 0;
        cudaEventElapsedTime(&milliseconds, start, stop);

        // Копируем результат обратно для верификации (опционально)
        cudaMemcpy(h_bodies.data(), d_bodies, size, cudaMemcpyDeviceToHost);

        double duration = milliseconds / 1000.0;
        std::cout << "N: " << n << " | CUDA Time: " << duration << "s" << std::endl;
        report << n << ",1," << duration << "\n"; // "1" здесь условно для совместимости со скриптом

        cudaFree(d_bodies);
        cudaEventDestroy(start);
        cudaEventDestroy(stop);
    }

    report.close();
    return 0;
}