#include "slog.h"

namespace slog {

LoggerManager::LoggerManager()
    : init_spdlog_pool_(false) {}

LoggerManager::~LoggerManager() {
    spdlog::shutdown();
}

void LoggerManager::init(const LoggerConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);

    spdlog::init_thread_pool(config.queue_size_, config.thread_count_);
    init_spdlog_pool_ = true;
    for (const auto& module_logger_config : config.module_logger_configs_) {
        createLogger(module_logger_config);
    }
}

// 获取指定模块的日志器
std::shared_ptr<spdlog::logger> LoggerManager::CreateLogger(const ModuleLoggerConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (loggers_.find(config.module_name_) == loggers_.end()) {
        createLogger(config);
    }
    return loggers_[config.module_name_];
}

std::shared_ptr<spdlog::logger> LoggerManager::CreateLogger(const std::string& module_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (loggers_.find(module_name) == loggers_.end()) {
        ModuleLoggerConfig config(module_name);
        createLogger(config);
    }
    return loggers_[module_name];
}

std::shared_ptr<spdlog::logger> LoggerManager::getLogger(const std::string& module_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (loggers_.find(module_name) != loggers_.end()) {
        return loggers_[module_name];
    }
    return nullptr;
}

void LoggerManager::setLogLevel(const std::string& module_name, spdlog::level::level_enum level) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (loggers_.find(module_name) != loggers_.end()) {
        loggers_[module_name]->set_level(level);
    }
}

std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> LoggerManager::getAllLoggers() {
    std::lock_guard<std::mutex> lock(mutex_);
    return loggers_;
}

void LoggerManager::createLogger(const ModuleLoggerConfig& config) {
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

    std::shared_ptr<spdlog::logger> logger;
    if (init_spdlog_pool_) {
        logger = std::make_shared<spdlog::async_logger>(config.module_name_, sinks.begin(), sinks.end(), spdlog::thread_pool());
    } else {
        logger = std::make_shared<spdlog::logger>(config.module_name_, sinks.begin(), sinks.end());
    }

    logger->set_level(config.log_level_);  // 默认等级
    logger->flush_on(config.log_level_);   // 自动刷新日志
    spdlog::register_logger(logger);

    loggers_[config.module_name_] = logger;
}

}  // namespace slog
