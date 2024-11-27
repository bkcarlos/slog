#ifndef __SLOG_SLOG_H__
#define __SLOG_SLOG_H__

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <string>

// 这样的使用方法更加简洁，但是性能会有所下降
#define SLOG_TRACE(module_name, ...)                                                \
    do {                                                                            \
        auto logger = slog::LoggerManager::getInstance().CreateLogger(module_name); \
        logger->trace(__VA_ARGS__);                                                 \
    } while (0)

#define SLOG_DEBUG(module_name, ...)                                                \
    do {                                                                            \
        auto logger = slog::LoggerManager::getInstance().CreateLogger(module_name); \
        logger->debug(__VA_ARGS__);                                                 \
    } while (0)

#define SLOG_INFO(module_name, ...)                                                 \
    do {                                                                            \
        auto logger = slog::LoggerManager::getInstance().CreateLogger(module_name); \
        logger->info(__VA_ARGS__);                                                  \
    } while (0)

#define SLOG_WARN(module_name, ...)                                                 \
    do {                                                                            \
        auto logger = slog::LoggerManager::getInstance().CreateLogger(module_name); \
        logger->warn(__VA_ARGS__);                                                  \
    } while (0)

#define SLOG_ERROR(module_name, ...)                                                \
    do {                                                                            \
        auto logger = slog::LoggerManager::getInstance().CreateLogger(module_name); \
        logger->error(__VA_ARGS__);                                                 \
    } while (0)

#define SLOG_CRITICAL(module_name, ...)                                             \
    do {                                                                            \
        auto logger = slog::LoggerManager::getInstance().CreateLogger(module_name); \
        logger->critical(__VA_ARGS__);                                              \
    } while (0)

// 这样的使用方法更加灵活，但是性能会有所提升
// auto logger = slog::LoggerManager::getInstance().getLogger("module_name");
// logger->trace("...");
// logger->debug("...");

namespace slog {

struct ModuleLoggerConfig final {
public:
    ModuleLoggerConfig(const std::string& module_name, spdlog::level::level_enum log_level = spdlog::level::info)
        : module_name_(module_name),
          log_level_(log_level),
          use_console_(false),
          use_file_(true),
          max_file_size_(100 * 1024),
          max_files_(5),
          pattern_("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v") {}

public:
    std::string module_name_;
    spdlog::level::level_enum log_level_;
    bool use_console_;
    bool use_file_;
    std::string file_name_;
    uint32_t max_file_size_;
    uint32_t max_files_;
    std::string daily_file_name_;
    std::string pattern_;
};

struct LoggerConfig final {
public:
    LoggerConfig(size_t queue_size = 8192, size_t thread_count = 1)
        : queue_size_(queue_size),
          thread_count_(thread_count) {}

public:
    size_t queue_size_;
    size_t thread_count_;
    std::vector<ModuleLoggerConfig> module_logger_configs_;
};

class LoggerManager final {
public:
    // 获取单例
    static LoggerManager& getInstance() {
        static LoggerManager instance;
        return instance;
    }

    void init(const LoggerConfig& config);

    std::shared_ptr<spdlog::logger> CreateLogger(const ModuleLoggerConfig& config);
    std::shared_ptr<spdlog::logger> CreateLogger(const std::string& module_name);

    // 获取指定模块的日志器,如果不存在返回nullptr
    std::shared_ptr<spdlog::logger> getLogger(const std::string& module_name);

    // 动态调整日志器的日志等级
    void setLogLevel(const std::string& module_name, spdlog::level::level_enum level);

    // 获取所有日志器
    std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> getAllLoggers();

private:
    // 私有构造函数
    LoggerManager();
    ~LoggerManager();

    // 禁止拷贝和赋值
    LoggerManager(const LoggerManager&) = delete;
    LoggerManager& operator=(const LoggerManager&) = delete;

    // 创建日志器
    void createLogger(const ModuleLoggerConfig& config);

private:
    std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> loggers_;
    std::mutex mutex_;
    bool init_spdlog_pool_;
};

}  // namespace slog

#endif  // __SLOG_SLOG_H__
