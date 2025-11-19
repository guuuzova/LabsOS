#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <chrono>

#include "thread.h"
#include "bigint.h"


struct ThreadData {
    const std::vector<BigInt>* my_numbers;
    size_t start;
    size_t end;
    BigInt sum;
};

void* thread_func(void* arg) {
    ThreadData* td = static_cast<ThreadData*>(arg);
    for (size_t i = td->start; i < td->end; ++i) {
        td->sum.Add((*td->my_numbers)[i]);
    }
    return nullptr;
}

int main(int argc, char* argv[]) {
    const size_t MEMORY_SYS = 20;
    if (argc != 4) {
        std::cerr << "Must be 3 argument";
        return 1;
    }
    size_t threadsCount = std::stoul(argv[1]);
    size_t memory = std::stoul(argv[2]);
    std::string my_file = argv[3];

    if (threadsCount == 0 || threadsCount > 10000) {
        std::cerr << "Incorrect threads count" << std::endl;
        return 1;
    }

    if (memory == 0 || memory > 10000000) {
        std::cerr << "Incorrect memory count" << std::endl;
        return 1;
    }

    if (memory < MEMORY_SYS) {
        std::cerr << "Memory limit too small" << std::endl;
        return 1;
    }
    auto start = std::chrono::steady_clock::now();

    const size_t bigint_size = sizeof(BigInt);
    const size_t usable_memory = memory - MEMORY_SYS;
    size_t max_numbers = (usable_memory * 1024ULL * 1024ULL) / bigint_size; //не хочу исправить!
    std::ifstream file(my_file);
    std::vector<BigInt> numbers;
    numbers.reserve(usable_memory);
    std::string line;
    while (numbers.size() < max_numbers && std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        if (line.length() > 128) line.resize(128);
        if (line.length() < 128) {
            line = std::string(128 - line.length(), '0') + line;
        }

        BigInt num;
        int res = num.ReadFromHex(line.c_str(), line.length());
        if (res != 0) {
            std::cerr << "Error: invalid hex number at line " << numbers.size() + 1 << "\n";
            return 1;
        }
        numbers.push_back(num);
    }
    file.close();


    size_t threads_n = std::min(threadsCount, numbers.size());
    std::vector<thread::Thread> threads;
    std::vector<ThreadData> threadData(threads_n);

    size_t base = numbers.size() / threads_n;
    size_t leftover = numbers.size() % threads_n;
    size_t current = 0;

    for (size_t i = 0; i < threads_n; ++i) {
        const size_t block = base + (i < leftover ? 1 : 0);
        
        threadData[i].my_numbers = &numbers;
        threadData[i].start = current;
        threadData[i].end = current + block;
        current += block;

        try {
            threads.emplace_back(thread_func);
            threads.back().Run(&threadData[i]);
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }
    BigInt total_sum;
    size_t total_count = numbers.size();
    for (size_t i = 0; i < threads_n; ++i) {
        try {
            threads[i].Join();
            total_sum.Add(threadData[i].sum);
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
    }
    BigInt average;
    uint64_t remainder = total_sum.Divide(static_cast<uint64_t>(total_count), average);
    if (remainder * 2 >= total_count) {
        average.Round();
    }
    average.Print();

    auto end = std::chrono::steady_clock::now();

    std::cout << "Algorithm was done in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << std::endl;
    return 0;
}



