#include "slog.h"

namespace slog {

// 获取指定模块的日志器
std::shared_ptr<Logger> LoggerManager::getLogger(const LoggerConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (loggers_.find(config.module_name_) == loggers_.end()) {
        createLogger(config);
    }
    return loggers_[config.module_name_];
}

std::shared_ptr<Logger> LoggerManager::getLogger(const std::string& module_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (loggers_.find(module_name) == loggers_.end()) {
        LoggerConfig config(module_name);
        createLogger(config);
    }
    return loggers_[module_name];
}

void LoggerManager::setLogLevel(const std::string& module_name, spdlog::level::level_enum level) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (loggers_.find(module_name) != loggers_.end()) {
        loggers_[module_name]->set_level(level);
    }
}

std::unordered_map<std::string, std::shared_ptr<Logger>> LoggerManager::getAllLoggers() {
    std::lock_guard<std::mutex> lock(mutex_);
    return loggers_;
}

void LoggerManager::createLogger(const LoggerConfig& config) {
    std::vector<spdlog::sink_ptr> sinks;
    sinks.reserve(2);
    if (config.use_console_) {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_pattern(config.pattern_);
        sinks.push_back(console_sink);
    }
    if (config.use_file_) {
        auto file_sink =
            std::make_shared<spdlog::sinks::rotating_file_sink_mt>(config.module_name_ + ".log", config.max_file_size_, config.max_files_);
        file_sink->set_pattern(config.pattern_);
        sinks.push_back(file_sink);
    }

    auto logger = std::make_shared<Logger>(config.module_name_, sinks.begin(), sinks.end());

    logger->set_level(config.log_level_);  // 默认等级
    logger->flush_on(config.log_level_);   // 自动刷新日志
    spdlog::register_logger(logger);

    loggers_[config.module_name_] = logger;
}

}  // namespace slog
