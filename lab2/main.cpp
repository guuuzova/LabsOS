#include "bigint.h"
#include "thread.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <cmath>
#include <chrono>

constexpr size_t MAX_LEN = 128;
constexpr size_t LINE_SIZE = MAX_LEN + 1; 

BigInt::BigInt global_sum;
size_t total_count = 0;

struct ThreadData {
    const std::vector<std::string>* hex_numbers;
    size_t start_index;
    size_t end_index;
};

void* thread_work(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    BigInt::BigInt local_sum;
    size_t local_count = 0;
    for (size_t i = data->start_index; i < data->end_index; ++i) {
        try {
            BigInt::BigInt num((*data->hex_numbers)[i]);
            local_sum += num;
            local_count++;
        } catch (const std::exception& e) {
            std::cerr << "Thread error" << e.what() << std::endl;
        }
    }
    {
        std::lock_guard<std::mutex> lock(BigInt::BigInt::sum_mutex);
        global_sum += local_sum;
        total_count += local_count;
    }
    return nullptr;
}


std::vector<std::string> read_data_with_memory_limit(const std::string& filename, size_t max_bytes) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::vector<std::string> numbers;
    std::string line;
    size_t current_mem_usage = 0;
    constexpr size_t NUM_STR_SIZE = MAX_LEN + 1; 

    while (std::getline(file, line)) {
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        
        if (line.empty()) continue;
        size_t line_mem = line.capacity() + sizeof(std::string) + sizeof(void*); 
        
        if (current_mem_usage + line_mem > max_bytes) {
            std::cerr << "Memory limit. Stopping file read." << std::endl;
            break;
        }

        numbers.push_back(line);
        current_mem_usage += line_mem;
    }
    
    if (numbers.empty()) {
        throw std::runtime_error("Error");
    }
    return numbers;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Error input" << std::endl;
        return 1;
    }
    const std::string filename = argv[1];
    int max_threads = 0;
    size_t max_bytes = 0;
    try {
        max_threads = std::stoi(argv[2]);
        max_bytes = std::stoull(argv[3]); 
    } catch (const std::exception& e) {
        std::cerr << "Invalid argument" << e.what() << std::endl;
        return 1;
    }

    if (max_threads <= 0 || max_bytes == 0) {
        std::cerr << "Error argument" << std::endl;
        return 1;
    }
    
    size_t max_threads = static_cast<size_t>(max_threads);

    std::vector<std::string> hex_numbers;
    try {
        hex_numbers = read_data_with_memory_limit(filename, max_bytes);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error file: " << e.what() << std::endl;
        return 1;
    }
    size_t num_numbers = hex_numbers.size();
    size_t num_my_threads = std::min(num_numbers, max_threads);
    if (num_my_threads == 0) {
        std::cerr << "No numbers to process" << std::endl;
        return 0;
    }
    auto start_time = std::chrono::high_resolution_clock::now();
    std::vector<ThreadData> all_data(num_my_threads);
    size_t base_chunk_size = num_numbers / num_my_threads;
    size_t remainder = num_numbers % num_my_threads;
    size_t current_start = 0;

    for (size_t i = 0; i < num_my_threads; ++i) {
        size_t chunk_size = base_chunk_size + (i < remainder ? 1 : 0);
        all_data[i] = {&hex_numbers, current_start, current_start + chunk_size};
        current_start += chunk_size;
    }
    std::vector<thread::Thread> threads;
    threads.reserve(num_my_threads);

    for (size_t i = 0; i < num_my_threads; ++i) {
        try {
            threads.emplace_back(thread::Thread(thread_work));
            threads.back().Run(&all_data[i]);
        } catch (const std::system_error& e) {
            std::cerr << "Error thread " << e.what() << std::endl;
        }
    }
    for (auto& t : threads) {
        try {
            t.Join();
        } catch (const std::system_error& e) {
            std::cerr << "Error joining thread: " << e.what() << std::endl;
        }
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto answer_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "time " << answer_time << " ms" << std::endl;

    if (total_count == 0) {
        std::cout << "No numbers" << std::endl;
        return 0;
    }
    std::cout << "Sum (HEX): " << global_sum << std::endl;
    std::cout << "Sum (DEC): " << global_sum.to_dec() << std::endl;
    uint64_t divisor = static_cast<uint64_t>(total_count);
    uint64_t half_divisor = divisor / 2;
    global_sum.add_word(half_divisor); 
    global_sum.divide_by_block(divisor); 
    std::cout << "Rounded: " << global_sum.to_dec() << std::endl;
    return 0;
}