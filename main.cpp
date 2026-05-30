#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>
#include <random>
#include <algorithm>
#include <iomanip>

class EntityClass {
public:
    float x, y, z, w;

    __declspec(noinline) float compute() const {
        return x * 1.5f + y * 2.5f + z * 3.5f + w * 4.5f;
    }
};

struct EntityStruct {
    float x, y, z, w;

    __declspec(noinline) float compute() const {
        return x * 1.5f + y * 2.5f + z * 3.5f + w * 4.5f;
    }
};

constexpr int Size = 10'000'000;
constexpr int Iterations = 5;

template <typename T>
void shuffle_vector(std::vector<T>& vec) {
    std::mt19937 g(42);
    std::shuffle(vec.begin(), vec.end(), g);
}

int main() {
    std::cout << "==========================================================================\n"
              << "   CACHE LOCALITY BENCHMARK: DATA-ORIENTED DESIGN (DOD) VS OBJECT-ORIENTED (OOD)\n"
              << "==========================================================================\n"
              << "Dataset size: " << Size << " elements\n\n"
              << "Memory Layout & Pattern Definitions:\n"
              << " -> CONTIGUOUS (Continuo): Elements are stored physically adjacent in RAM\n"
              << "    with absolutely zero gaps (like houses in a row). Vector of Structs.\n"
              << " -> SEQUENTIAL (Secuencial): Memory is read in order, from index 0 to N.\n"
              << "    Allows the CPU to prefetch next elements into L1/L2/L3 caches.\n"
              << " -> POINTER CHASING (Indireccion): Storing pointer addresses instead of values.\n"
              << "    Forces CPU to jump to dynamic memory locations (Heap), causing Cache Misses.\n\n"
              << "Allocating and preparing memory...\n" << std::endl;

    auto dod_array = std::make_unique<std::vector<EntityStruct>>(Size);
    for (int i = 0; i < Size; ++i) {
        (*dod_array)[i] = EntityStruct{ (float)i, (float)i * 2.f, (float)i * 3.f, (float)i * 4.f };
    }

    auto ood_array_seq = std::make_unique<std::vector<EntityClass*>>(Size);
    for (int i = 0; i < Size; ++i) {
        (*ood_array_seq)[i] = new EntityClass{ (float)i, (float)i * 2.f, (float)i * 3.f, (float)i * 4.f };
    }

    auto ood_array_shuffled = std::make_unique<std::vector<EntityClass*>>(Size);
    for (int i = 0; i < Size; ++i) {
        (*ood_array_shuffled)[i] = (*ood_array_seq)[i];
    }
    shuffle_vector(*ood_array_shuffled);

    std::vector<int> random_indices(Size);
    std::iota(random_indices.begin(), random_indices.end(), 0);
    shuffle_vector(random_indices);

    std::cout << "Allocation complete. Starting benchmarks...\n" << std::endl;

    volatile float global_sum = 0.0f;

    std::cout << "Running DOD (Structs) - Sequential Access... " << std::flush;
    double dod_seq_time = 0.0;
    for (int iter = 0; iter < Iterations; ++iter) {
        auto start = std::chrono::high_resolution_clock::now();
        float sum = 0.0f;
        for (int i = 0; i < Size; ++i) {
            sum += (*dod_array)[i].compute();
        }
        auto end = std::chrono::high_resolution_clock::now();
        global_sum = sum;
        dod_seq_time += std::chrono::duration<double, std::milli>(end - start).count();
    }
    dod_seq_time /= Iterations;
    std::cout << std::fixed << std::setprecision(2) << dod_seq_time << " ms" << std::endl;

    std::cout << "Running OOD (Classes) - Sequential References... " << std::flush;
    double ood_seq_time = 0.0;
    for (int iter = 0; iter < Iterations; ++iter) {
        auto start = std::chrono::high_resolution_clock::now();
        float sum = 0.0f;
        for (int i = 0; i < Size; ++i) {
            sum += (*ood_array_seq)[i]->compute();
        }
        auto end = std::chrono::high_resolution_clock::now();
        global_sum = sum;
        ood_seq_time += std::chrono::duration<double, std::milli>(end - start).count();
    }
    ood_seq_time /= Iterations;
    std::cout << ood_seq_time << " ms" << std::endl;

    std::cout << "Running OOD (Classes) - Shuffled References (Cache Misses)... " << std::flush;
    double ood_shuffled_time = 0.0;
    for (int iter = 0; iter < Iterations; ++iter) {
        auto start = std::chrono::high_resolution_clock::now();
        float sum = 0.0f;
        for (int i = 0; i < Size; ++i) {
            sum += (*ood_array_shuffled)[i]->compute();
        }
        auto end = std::chrono::high_resolution_clock::now();
        global_sum = sum;
        ood_shuffled_time += std::chrono::duration<double, std::milli>(end - start).count();
    }
    ood_shuffled_time /= Iterations;
    std::cout << ood_shuffled_time << " ms" << std::endl;

    std::cout << "Running DOD (Structs) - Random Index Access (Cache Misses)... " << std::flush;
    double dod_random_time = 0.0;
    for (int iter = 0; iter < Iterations; ++iter) {
        auto start = std::chrono::high_resolution_clock::now();
        float sum = 0.0f;
        for (int i = 0; i < Size; ++i) {
            sum += (*dod_array)[random_indices[i]].compute();
        }
        auto end = std::chrono::high_resolution_clock::now();
        global_sum = sum;
        dod_random_time += std::chrono::duration<double, std::milli>(end - start).count();
    }
    dod_random_time /= Iterations;
    std::cout << dod_random_time << " ms" << std::endl;

    std::cout << "==========================================================================\n"
              << "                               BENCHMARK RESULTS                          \n"
              << "==========================================================================\n"
              << std::left << std::setw(50) << "Benchmark Type" << " | "
              << std::setw(10) << "Time (ms)" << " | "
              << std::setw(15) << "Relative Speed" << "\n"
              << "--------------------------------------------------------------------------\n"
              << std::left << std::setw(50) << "DOD (Structs) - Sequential Access" << " | "
              << std::right << std::setw(10) << dod_seq_time << " | "
              << std::left << std::setw(15) << "1.00x (Baseline)" << "\n"
              << std::left << std::setw(50) << "OOD (Classes) - Sequential References" << " | "
              << std::right << std::setw(10) << ood_seq_time << " | "
              << std::right << std::setw(10) << (ood_seq_time / dod_seq_time) << "x slower\n"
              << std::left << std::setw(50) << "OOD (Classes) - Shuffled References (Cache Misses)" << " | "
              << std::right << std::setw(10) << ood_shuffled_time << " | "
              << std::right << std::setw(10) << (ood_shuffled_time / dod_seq_time) << "x slower\n"
              << std::left << std::setw(50) << "DOD (Structs) - Random Index Access (Cache Misses)" << " | "
              << std::right << std::setw(10) << dod_random_time << " | "
              << std::right << std::setw(10) << (dod_random_time / dod_seq_time) << "x slower\n"
              << "==========================================================================\n";

    double percent_seq = ((ood_seq_time - dod_seq_time) / dod_seq_time) * 100.0;
    double percent_shuffled = ((ood_shuffled_time - dod_seq_time) / dod_seq_time) * 100.0;

    std::cout << "* DOD Sequential is " << std::fixed << std::setprecision(0) << percent_seq << "% FASTER than traditional OOD (Sequential references).\n"
              << "* DOD Sequential is " << std::fixed << std::setprecision(0) << percent_shuffled << "% FASTER than OOD with fragmented references (Cache Misses).\n"
              << "==========================================================================\n" << std::endl;

    for (int i = 0; i < Size; ++i) {
        delete (*ood_array_seq)[i];
    }

    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    return 0;
}
