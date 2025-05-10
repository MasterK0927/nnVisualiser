/**
 * @file Logger.hpp
 * @brief Logging system for the Neural Network Visualizer
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <memory>
#include <fstream>
#include <mutex>
#include <sstream>

#ifdef HAS_SPDLOG
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#endif

#ifdef HAS_FMT
#include <fmt/format.h>
#endif

namespace nnv {
namespace utils {

/**
 * @brief Log levels for the logging system
 */
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

/**
 * @brief Main logging class
 */
class Logger {
public:
    /**
     * @brief Initialize the logging system
     * @param logFile Optional log file path
     * @param level Minimum log level to output
     */
    static void initialize(const std::string& logFile = "", LogLevel level = LogLevel::Info);
    
    /**
     * @brief Shutdown the logging system
     */
    static void shutdown();
    
    /**
     * @brief Set the minimum log level
     * @param level New minimum log level
     */
    static void setLevel(LogLevel level);
    
    /**
     * @brief Log a message with specified level
     * @param level Log level
     * @param message Message to log
     */
    static void log(LogLevel level, const std::string& message);
    
    /**
     * @brief Log a debug message
     * @param message Message to log
     */
    static void debug(const std::string& message);
    
    /**
     * @brief Log an info message
     * @param message Message to log
     */
    static void info(const std::string& message);
    
    /**
     * @brief Log a warning message
     * @param message Message to log
     */
    static void warning(const std::string& message);
    
    /**
     * @brief Log an error message
     * @param message Message to log
     */
    static void error(const std::string& message);
    
    /**
     * @brief Log a critical message
     * @param message Message to log
     */
    static void critical(const std::string& message);
    
    /**
     * @brief Template function for formatted logging
     */
    template<typename... Args>
    static void log(LogLevel level, const std::string& format, Args&&... args) {
#ifdef HAS_FMT
        log(level, fmt::format(format, std::forward<Args>(args)...));
#else
        // Simple fallback formatting
        std::ostringstream oss;
        oss << format;
        log(level, oss.str());
#endif
    }
    
    template<typename... Args>
    static void debug(const std::string& format, Args&&... args) {
        log(LogLevel::Debug, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    static void info(const std::string& format, Args&&... args) {
        log(LogLevel::Info, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    static void warning(const std::string& format, Args&&... args) {
        log(LogLevel::Warning, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    static void error(const std::string& format, Args&&... args) {
        log(LogLevel::Error, format, std::forward<Args>(args)...);
    }
    
    template<typename... Args>
    static void critical(const std::string& format, Args&&... args) {
        log(LogLevel::Critical, format, std::forward<Args>(args)...);
    }

private:
    static std::unique_ptr<Logger> instance_;
    static std::mutex mutex_;
    
    Logger() = default;
    ~Logger() = default;
    
    void initializeImpl(const std::string& logFile, LogLevel level);
    void shutdownImpl();
    void setLevelImpl(LogLevel level);
    void logImpl(LogLevel level, const std::string& message);
    
    std::string levelToString(LogLevel level) const;
    std::string getCurrentTimestamp() const;
    
#ifdef HAS_SPDLOG
    std::shared_ptr<spdlog::logger> spdlogger_;
#else
    std::unique_ptr<std::ofstream> fileStream_;
    LogLevel currentLevel_ = LogLevel::Info;
    bool consoleOutput_ = true;
#endif
};

} // namespace utils
} // namespace nnv

// Convenience macros for logging
#define NNV_LOG_DEBUG(msg, ...) nnv::utils::Logger::debug(msg, ##__VA_ARGS__)
#define NNV_LOG_INFO(msg, ...) nnv::utils::Logger::info(msg, ##__VA_ARGS__)
#define NNV_LOG_WARNING(msg, ...) nnv::utils::Logger::warning(msg, ##__VA_ARGS__)
#define NNV_LOG_ERROR(msg, ...) nnv::utils::Logger::error(msg, ##__VA_ARGS__)
#define NNV_LOG_CRITICAL(msg, ...) nnv::utils::Logger::critical(msg, ##__VA_ARGS__)
