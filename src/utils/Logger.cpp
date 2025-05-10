/**
 * @file Logger.cpp
 * @brief Implementation of the logging system
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include "utils/Logger.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>

namespace nnv {
namespace utils {

// Static member definitions
std::unique_ptr<Logger> Logger::instance_ = nullptr;
std::mutex Logger::mutex_;

void Logger::initialize(const std::string& logFile, LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!instance_) {
        instance_ = std::unique_ptr<Logger>(new Logger());
        instance_->initializeImpl(logFile, level);
    }
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_) {
        instance_->shutdownImpl();
        instance_.reset();
    }
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_) {
        instance_->setLevelImpl(level);
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_) {
        instance_->logImpl(level, message);
    }
}

void Logger::debug(const std::string& message) {
    log(LogLevel::Debug, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::Info, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::Warning, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::Error, message);
}

void Logger::critical(const std::string& message) {
    log(LogLevel::Critical, message);
}

void Logger::initializeImpl(const std::string& logFile, LogLevel level) {
#ifdef HAS_SPDLOG
    try {
        std::vector<spdlog::sink_ptr> sinks;
        
        // Console sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(static_cast<spdlog::level::level_enum>(level));
        sinks.push_back(console_sink);
        
        // File sink (if specified)
        if (!logFile.empty()) {
            auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFile, true);
            file_sink->set_level(static_cast<spdlog::level::level_enum>(level));
            sinks.push_back(file_sink);
        }
        
        spdlogger_ = std::make_shared<spdlog::logger>("nnv", sinks.begin(), sinks.end());
        spdlogger_->set_level(static_cast<spdlog::level::level_enum>(level));
        spdlogger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        
        spdlog::register_logger(spdlogger_);
        
    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
    }
#else
    currentLevel_ = level;
    consoleOutput_ = true;
    
    if (!logFile.empty()) {
        fileStream_ = std::make_unique<std::ofstream>(logFile, std::ios::app);
        if (!fileStream_->is_open()) {
            std::cerr << "Failed to open log file: " << logFile << std::endl;
            fileStream_.reset();
        }
    }
#endif
}

void Logger::shutdownImpl() {
#ifdef HAS_SPDLOG
    if (spdlogger_) {
        spdlogger_->flush();
        spdlog::drop("nnv");
        spdlogger_.reset();
    }
#else
    if (fileStream_) {
        fileStream_->flush();
        fileStream_->close();
        fileStream_.reset();
    }
#endif
}

void Logger::setLevelImpl(LogLevel level) {
#ifdef HAS_SPDLOG
    if (spdlogger_) {
        spdlogger_->set_level(static_cast<spdlog::level::level_enum>(level));
    }
#else
    currentLevel_ = level;
#endif
}

void Logger::logImpl(LogLevel level, const std::string& message) {
#ifdef HAS_SPDLOG
    if (spdlogger_) {
        switch (level) {
            case LogLevel::Debug:
                spdlogger_->debug(message);
                break;
            case LogLevel::Info:
                spdlogger_->info(message);
                break;
            case LogLevel::Warning:
                spdlogger_->warn(message);
                break;
            case LogLevel::Error:
                spdlogger_->error(message);
                break;
            case LogLevel::Critical:
                spdlogger_->critical(message);
                break;
        }
    }
#else
    if (level < currentLevel_) {
        return;
    }
    
    std::string timestamp = getCurrentTimestamp();
    std::string levelStr = levelToString(level);
    std::string logLine = "[" + timestamp + "] [" + levelStr + "] " + message;
    
    if (consoleOutput_) {
        if (level >= LogLevel::Error) {
            std::cerr << logLine << std::endl;
        } else {
            std::cout << logLine << std::endl;
        }
    }
    
    if (fileStream_ && fileStream_->is_open()) {
        *fileStream_ << logLine << std::endl;
        fileStream_->flush();
    }
#endif
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

std::string Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

} // namespace utils
} // namespace nnv
