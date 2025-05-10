/**
 * @file Common.hpp
 * @brief Common utilities and macros for the Neural Network Visualizer
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <random>

// Version information
#define NNV_VERSION_MAJOR 1
#define NNV_VERSION_MINOR 0
#define NNV_VERSION_PATCH 0
#define NNV_VERSION_STRING "1.0.0"

// Platform detection
#ifdef _WIN32
    #define NNV_PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define NNV_PLATFORM_MACOS
#elif defined(__linux__)
    #define NNV_PLATFORM_LINUX
#endif

// Compiler detection
#ifdef _MSC_VER
    #define NNV_COMPILER_MSVC
#elif defined(__GNUC__)
    #define NNV_COMPILER_GCC
#elif defined(__clang__)
    #define NNV_COMPILER_CLANG
#endif

// Debug/Release detection
#ifdef NDEBUG
    #define NNV_RELEASE
#else
    #define NNV_DEBUG
#endif

// Utility macros
#define NNV_UNUSED(x) ((void)(x))
#define NNV_STRINGIFY(x) #x
#define NNV_TOSTRING(x) NNV_STRINGIFY(x)

// Memory management
#define NNV_MAKE_UNIQUE(T, ...) std::make_unique<T>(__VA_ARGS__)
#define NNV_MAKE_SHARED(T, ...) std::make_shared<T>(__VA_ARGS__)

// Disable copy and move constructors/operators
#define NNV_DISABLE_COPY(ClassName) \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;

#define NNV_DISABLE_MOVE(ClassName) \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete;

#define NNV_DISABLE_COPY_AND_MOVE(ClassName) \
    NNV_DISABLE_COPY(ClassName) \
    NNV_DISABLE_MOVE(ClassName)

// Assert macros
#ifdef NNV_DEBUG
    #include <cassert>
    #define NNV_ASSERT(condition) assert(condition)
    #define NNV_ASSERT_MSG(condition, message) assert((condition) && (message))
#else
    #define NNV_ASSERT(condition) ((void)0)
    #define NNV_ASSERT_MSG(condition, message) ((void)0)
#endif

namespace nnv {
namespace utils {

// Time utilities
using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
using Duration = Clock::duration;

inline TimePoint now() {
    return Clock::now();
}

inline double durationMs(const TimePoint& start, const TimePoint& end) {
    return std::chrono::duration<double, std::milli>(end - start).count();
}

inline double durationSeconds(const TimePoint& start, const TimePoint& end) {
    return std::chrono::duration<double>(end - start).count();
}

// Random number generation
class Random {
public:
    Random() : generator_(std::random_device{}()) {}
    explicit Random(std::uint32_t seed) : generator_(seed) {}
    
    float uniform(float min = 0.0f, float max = 1.0f) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(generator_);
    }
    
    double uniform(double min = 0.0, double max = 1.0) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(generator_);
    }
    
    int uniform(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(generator_);
    }
    
    float normal(float mean = 0.0f, float stddev = 1.0f) {
        std::normal_distribution<float> dist(mean, stddev);
        return dist(generator_);
    }
    
    double normal(double mean = 0.0, double stddev = 1.0) {
        std::normal_distribution<double> dist(mean, stddev);
        return dist(generator_);
    }
    
    void seed(std::uint32_t seed) {
        generator_.seed(seed);
    }
    
private:
    std::mt19937 generator_;
};

// Global random instance
extern Random g_random;

// String utilities
std::string toLower(const std::string& str);
std::string toUpper(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);
std::string trim(const std::string& str);
bool startsWith(const std::string& str, const std::string& prefix);
bool endsWith(const std::string& str, const std::string& suffix);

// File path utilities
std::string getFileExtension(const std::string& filename);
std::string getFileName(const std::string& path);
std::string getDirectoryPath(const std::string& path);
bool fileExists(const std::string& path);
bool directoryExists(const std::string& path);

// Math utilities
template<typename T>
constexpr T clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

template<typename T>
constexpr T lerp(T a, T b, T t) {
    return a + t * (b - a);
}

template<typename T>
constexpr T map(T value, T fromMin, T fromMax, T toMin, T toMax) {
    return toMin + (value - fromMin) * (toMax - toMin) / (fromMax - fromMin);
}

} // namespace utils
} // namespace nnv
