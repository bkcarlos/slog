#include "slog.h"
#include <chrono>
#include <vector>
#include <thread>
#include <algorithm>
#include <numeric>
#include <iostream>

void log_statistics(const std::vector<double>& times, const std::string& test_type) {
    // 计算平均耗时
    double average = std::accumulate(times.begin(), times.end(), 0.0) / times.size();

    // 计算95%的耗时分布
    auto sorted_times = times;
    std::sort(sorted_times.begin(), sorted_times.end());
    double percentile_95 = sorted_times[static_cast<size_t>(0.95 * sorted_times.size())];

    spdlog::info("{} Test - Average time per log: {:.6f} ms", test_type, average);
    spdlog::info("{} Test - 95% time per log: {:.6f} ms", test_type, percentile_95);
}

void test_single_thread_logging(size_t log_count) {
    auto logger = slog::LoggerManager::getInstance().getLogger("single_thread");

    std::vector<double> log_times;
    log_times.reserve(log_count);

    for (size_t i = 0; i < log_count; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        logger->info("Logging message number: {}", i);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> log_time = end - start;
        log_times.push_back(log_time.count());
    }

    log_statistics(log_times, "Single-threaded");
}


void test_single_thread_logging_define(size_t log_count) {

    std::vector<double> log_times;
    log_times.reserve(log_count);

    for (size_t i = 0; i < log_count; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        SLOG_INFO("single_thread", "Logging message number: {}", i);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> log_time = end - start;
        log_times.push_back(log_time.count());
    }

    log_statistics(log_times, "Single-threaded-define");
}

void test_multi_thread_logging(size_t log_count, size_t thread_count) {
    auto async_logger = slog::LoggerManager::getInstance().getLogger("multi_thread");
    std::vector<std::vector<double>> thread_log_times(thread_count);

    auto worker = [&async_logger](size_t thread_id, size_t log_count, std::vector<double>& log_times) {
        log_times.reserve(log_count);
        for (size_t i = 0; i < log_count; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            async_logger->info("Thread {}: Logging message number: {}", thread_id, i);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> log_time = end - start;
            log_times.push_back(log_time.count());
        }
    };

    std::vector<std::thread> threads;
    for (size_t t = 0; t < thread_count; ++t) {
        threads.emplace_back(worker, t, log_count, std::ref(thread_log_times[t]));
    }
    for (auto& thread : threads) {
        thread.join();
    }

    // 汇总所有线程的日志时间
    std::vector<double> all_log_times;
    for (const auto& times : thread_log_times) {
        all_log_times.insert(all_log_times.end(), times.begin(), times.end());
    }

    log_statistics(all_log_times, "Multi-threaded");
}

int main() {
    size_t log_count = 100 * 1e4;  // 每线程日志条数
    size_t thread_count = 4;       // 线程数

    std::cout << "Starting single-threaded logging test..." << std::endl;
    test_single_thread_logging(log_count);

    std::cout << "Starting multi-threaded logging test..." << std::endl;
    test_multi_thread_logging(log_count, thread_count);

    std::cout << "Starting single-threaded logging test with define..." << std::endl;
    test_single_thread_logging_define(log_count);

    std::cout << "Performance test completed." << std::endl;
    return 0;
}