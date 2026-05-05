#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>
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

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const float G = 6.674e-11f;
    const float dt = 0.01f;
    std::vector<int> sizes = {200, 400, 800, 1200, 1600, 2000};

    std::ofstream report;
    if (rank == 0) {
        report.open("benchmark_results_mpi.csv", std::ios::app);

        std::ifstream check_file("benchmark_results_mpi.csv");
        check_file.seekg(0, std::ios::end);
        if (check_file.tellg() <= 5) {
            report << "Size,Processes,Time_sec\n";
        }

        std::cout << "=== MPI N-Body Simulation ===" << std::endl;
        std::cout << "Processes: " << size << std::endl;
    }

    for (int n : sizes) {
        if (n % size != 0) continue;

        int local_n = n / size;
        std::vector<Body> all_bodies(n);
        std::vector<Body> local_bodies(local_n);

        if (rank == 0) init_bodies(all_bodies, n);

        MPI_Bcast(all_bodies.data(), n * (int)sizeof(Body), MPI_BYTE, 0, MPI_COMM_WORLD);

        double start = MPI_Wtime();

        for (int i = 0; i < local_n; ++i) {
            int global_i = rank * local_n + i;
            float fx = 0, fy = 0, fz = 0;
            for (int j = 0; j < n; ++j) {
                if (global_i == j) continue;
                float dx = all_bodies[j].x - all_bodies[global_i].x;
                float dy = all_bodies[j].y - all_bodies[global_i].y;
                float dz = all_bodies[j].z - all_bodies[global_i].z;
                float distSq = dx*dx + dy*dy + dz*dz + 1e-4f;
                float invDist3 = 1.0f / (distSq * sqrtf(distSq));

                float s = all_bodies[j].mass * invDist3;

                fx += dx * s; fy += dy * s; fz += dz * s;
            }
            local_bodies[i] = all_bodies[global_i];
            local_bodies[i].vx += dt * G * fx;
            local_bodies[i].vy += dt * G * fy;
            local_bodies[i].vz += dt * G * fz;
            local_bodies[i].x += local_bodies[i].vx * dt;
            local_bodies[i].y += local_bodies[i].vy * dt;
            local_bodies[i].z += local_bodies[i].vz * dt;
        }

        MPI_Allgather(local_bodies.data(), local_n * (int)sizeof(Body), MPI_BYTE,
                      all_bodies.data(), local_n * (int)sizeof(Body), MPI_BYTE,
                      MPI_COMM_WORLD);

        double end = MPI_Wtime();

        if (rank == 0) {
            double duration = end - start;
            std::cout << "N: " << std::setw(4) << n << " | Time: " << std::fixed << std::setprecision(6) << duration << "s" << std::endl;
            report << n << "," << size << "," << duration << "\n";
        }
    }

    if (rank == 0) report.close();
    MPI_Finalize();
    return 0;
}
