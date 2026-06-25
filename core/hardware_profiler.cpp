#include "hardware_profiler.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>

double measure_memory_bandwidth_multithread() {

    const size_t N = 512 * 1024 * 1024; // 512M floats
    const int iterations = 20;

    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0)
        num_threads = 4;

    float* a = (float*)aligned_alloc(64, N * sizeof(float));
    float* b = (float*)aligned_alloc(64, N * sizeof(float));

    for (size_t i = 0; i < N; ++i)
        a[i] = static_cast<float>(i);

    auto worker = [&](size_t start, size_t end) {
        for (int it = 0; it < iterations; ++it) {
            for (size_t i = start; i < end; ++i) {
                b[i] = a[i];
            }
        }
    };

    std::vector<std::thread> threads;
    size_t chunk = N / num_threads;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (unsigned int t = 0; t < num_threads; ++t) {
        size_t start_idx = t * chunk;
        size_t end_idx = (t == num_threads - 1) ? N : start_idx + chunk;
        threads.emplace_back(worker, start_idx, end_idx);
    }

    for (auto& th : threads)
        th.join();

    auto end_time = std::chrono::high_resolution_clock::now();

    double elapsed_sec =
        std::chrono::duration<double>(end_time - start_time).count();

    double bytes = 2.0 * N * sizeof(float) * iterations;
    double bandwidth = bytes / elapsed_sec / 1e9;

    free(a);
    free(b);

    return bandwidth;
}

HardwareProfile profile_hardware() {

    HardwareProfile profile;

    std::cout << "Measuring CPU memory bandwidth...\n";

    profile.dram_bandwidth_gbps =
        measure_memory_bandwidth_multithread();

    std::cout << "Measured CPU Bandwidth: "
              << profile.dram_bandwidth_gbps
              << " GB/s\n";

    return profile;
}