#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>
#include <chrono>
#include <vector>
#include <thread>
#include <algorithm>
#include <numeric>

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
    auto logger = spdlog::rotating_logger_mt("single_thread", "single_thread_log.txt", 5 * 1024 * 1024, 5);
    logger->set_pattern("%Y-%m-%d %H:%M:%S.%e [%l] %v");

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

void test_multi_thread_logging(size_t log_count, size_t thread_count) {
    auto async_logger = spdlog::create_async<spdlog::sinks::basic_file_sink_mt>("async_logger", "multi_thread_log.txt");
    async_logger->set_pattern("%Y-%m-%d %H:%M:%S.%e [%l] %v");

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
    spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e [%l] %v");
    spdlog::set_level(spdlog::level::info);

    size_t log_count = 100 * 1e4;  // 每线程日志条数
    size_t thread_count = 4;       // 线程数

    spdlog::info("Starting single-threaded logging test...");
    test_single_thread_logging(log_count);

    spdlog::info("Starting multi-threaded logging test...");
    test_multi_thread_logging(log_count, thread_count);

    spdlog::info("Performance test completed.");
    return 0;
}