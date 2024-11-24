#ifndef __SLOG_SLOG_H__
#define __SLOG_SLOG_H__

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <string>

// 这样的使用方法更加简洁，但是性能会有所下降
#define SLOG_TRACE(module_name, ...)                                                  \
    do {                                                                              \
        auto logger = slog::LoggerManager::getInstance().getLogger(module_name); \
        if (logger->should_log(spdlog::level::trace)) {                               \
            logger->trace(__VA_ARGS__);                                               \
        }                                                                             \
    } while (0)

#define SLOG_DEBUG(module_name, ...)                                                  \
    do {                                                                              \
        auto logger = slog::LoggerManager::getInstance().getLogger(module_name); \
        if (logger->should_log(spdlog::level::debug)) {                               \
            logger->debug(__VA_ARGS__);                                               \
        }                                                                             \
    } while (0)

#define SLOG_INFO(module_name, ...)                                                   \
    do {                                                                              \
        auto logger = slog::LoggerManager::getInstance().getLogger(module_name); \
        if (logger->should_log(spdlog::level::info)) {                                \
            logger->info(__VA_ARGS__);                                                \
        }                                                                             \
    } while (0)

#define SLOG_WARN(module_name, ...)                                                   \
    do {                                                                              \
        auto logger = slog::LoggerManager::getInstance().getLogger(module_name); \
        if (logger->should_log(spdlog::level::warn)) {                                \
            logger->warn(__VA_ARGS__);                                                \
        }                                                                             \
    } while (0)

#define SLOG_ERROR(module_name, ...)                                                  \
    do {                                                                              \
        auto logger = slog::LoggerManager::getInstance().getLogger(module_name); \
        if (logger->should_log(spdlog::level::err)) {                                 \
            logger->error(__VA_ARGS__);                                               \
        }                                                                             \
    } while (0)

#define SLOG_CRITICAL(module_name, ...)                                               \
    do {                                                                              \
        auto logger = slog::LoggerManager::getInstance().getLogger(module_name); \
        if (logger->should_log(spdlog::level::critical)) {                            \
            logger->critical(__VA_ARGS__);                                            \
        }                                                                             \
    } while (0)

// 这样的使用方法更加灵活，但是性能会有所提升
// auto logger = slog::LoggerManager::getInstance().getLogger("module_name");
// logger->trace("...");
// logger->debug("...");

namespace slog {

struct LoggerConfig final {
public:
    LoggerConfig(const std::string& module_name, spdlog::level::level_enum log_level = spdlog::level::info)
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

class Logger final : public spdlog::logger {
public:
    // 构造函数，复用 spdlog::logger 的构造逻辑
    using spdlog::logger::logger;

    // 定义 info 方法，加入 should_log 检查
    template <typename... Args>
    void info(const char* fmt, const Args&... args) {
        if (should_log(spdlog::level::info)) {
            log(spdlog::level::info, fmt, args...);
        }
    }

    // 定义 debug 方法，加入 should_log 检查
    template <typename... Args>
    void debug(const char* fmt, const Args&... args) {
        if (should_log(spdlog::level::debug)) {
            log(spdlog::level::debug, fmt, args...);
        }
    }

    // 定义 warn 方法，加入 should_log 检查
    template <typename... Args>
    void warn(const char* fmt, const Args&... args) {
        if (should_log(spdlog::level::warn)) {
            log(spdlog::level::warn, fmt, args...);
        }
    }

    // 定义 error 方法，加入 should_log 检查
    template <typename... Args>
    void error(const char* fmt, const Args&... args) {
        if (should_log(spdlog::level::err)) {
            log(spdlog::level::err, fmt, args...);
        }
    }

    // 定义 critical 方法，加入 should_log 检查
    template <typename... Args>
    void critical(const char* fmt, const Args&... args) {
        if (should_log(spdlog::level::critical)) {
            log(spdlog::level::critical, fmt, args...);
        }
    }
};

class LoggerManager final {
public:
    // 获取单例
    static LoggerManager& getInstance() {
        static LoggerManager instance;
        return instance;
    }

    // 获取指定模块的日志器
    std::shared_ptr<Logger> getLogger(const LoggerConfig& config);
    std::shared_ptr<Logger> getLogger(const std::string& module_name);

    // 动态调整日志器的日志等级
    void setLogLevel(const std::string& module_name, spdlog::level::level_enum level);

    // 获取所有日志器
    std::unordered_map<std::string, std::shared_ptr<Logger>> getAllLoggers();

private:
    // 私有构造函数
    LoggerManager() = default;
    ~LoggerManager() = default;

    // 禁止拷贝和赋值
    LoggerManager(const LoggerManager&) = delete;
    LoggerManager& operator=(const LoggerManager&) = delete;

    // 创建日志器
    void createLogger(const LoggerConfig& config);

private:
    std::unordered_map<std::string, std::shared_ptr<Logger>> loggers_;
    std::mutex mutex_;
};

}  // namespace slog

#endif  // __SLOG_SLOG_H__
