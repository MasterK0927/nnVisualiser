/**
 * @file WeightInitializers.hpp
 * @brief Weight initialization strategies for neural networks
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <vector>
#include <functional>
#include <random>
#include <cmath>

#include "core/Types.hpp"
#include "utils/Common.hpp"

namespace nnv {
namespace core {

/**
 * @brief Weight initializers namespace
 */
namespace initializer {

/**
 * @brief Xavier/Glorot initialization
 * @tparam T Numeric type
 * @param fanIn Number of input connections
 * @param fanOut Number of output connections
 * @return Random value from uniform distribution
 */
template<typename T>
T xavier(std::size_t fanIn, std::size_t fanOut) {
    T limit = std::sqrt(T{6} / (fanIn + fanOut));
    return utils::g_random.uniform(-limit, limit);
}

/**
 * @brief He initialization (for ReLU activations)
 * @tparam T Numeric type
 * @param fanIn Number of input connections
 * @return Random value from normal distribution
 */
template<typename T>
T he(std::size_t fanIn) {
    T stddev = std::sqrt(T{2} / fanIn);
    return utils::g_random.normal(T{0}, stddev);
}

/**
 * @brief Random uniform initialization
 * @tparam T Numeric type
 * @param min Minimum value
 * @param max Maximum value
 * @return Random value from uniform distribution
 */
template<typename T>
T randomUniform(T min = T{-1}, T max = T{1}) {
    return utils::g_random.uniform(min, max);
}

/**
 * @brief Random normal initialization
 * @tparam T Numeric type
 * @param mean Mean value
 * @param stddev Standard deviation
 * @return Random value from normal distribution
 */
template<typename T>
T randomNormal(T mean = T{0}, T stddev = T{1}) {
    return utils::g_random.normal(mean, stddev);
}

/**
 * @brief Constant initialization
 * @tparam T Numeric type
 * @param value Constant value
 * @return Constant value
 */
template<typename T>
T constant(T value = T{0}) {
    return value;
}

/**
 * @brief Zero initialization
 * @tparam T Numeric type
 * @return Zero
 */
template<typename T>
T zeros() {
    return T{0};
}

/**
 * @brief One initialization
 * @tparam T Numeric type
 * @return One
 */
template<typename T>
T ones() {
    return T{1};
}

/**
 * @brief LeCun initialization
 * @tparam T Numeric type
 * @param fanIn Number of input connections
 * @return Random value from normal distribution
 */
template<typename T>
T lecun(std::size_t fanIn) {
    T stddev = std::sqrt(T{1} / fanIn);
    return utils::g_random.normal(T{0}, stddev);
}

/**
 * @brief Orthogonal initialization
 * @tparam T Numeric type
 * @param shape Shape of the weight matrix
 * @param gain Scaling factor
 * @return Orthogonal matrix
 */
template<typename T>
std::vector<std::vector<T>> orthogonal(std::pair<std::size_t, std::size_t> shape, T gain = T{1}) {
    // This is a simplified version that doesn't guarantee orthogonality
    // A full implementation would use QR decomposition
    std::vector<std::vector<T>> matrix(shape.first, std::vector<T>(shape.second));
    
    for (auto& row : matrix) {
        for (auto& val : row) {
            val = utils::g_random.normal();
        }
    }
    
    // Scale by gain
    for (auto& row : matrix) {
        for (auto& val : row) {
            val *= gain;
        }
    }
    
    return matrix;
}

} // namespace initializer

/**
 * @brief Weight initializer factory
 */
class InitializerFactory {
public:
    /**
     * @brief Get initializer function by type
     * @tparam T Numeric type
     * @param type Initialization type
     * @return Initializer function
     */
    template<typename T>
    static std::function<T(std::size_t, std::size_t)> getFunction(InitializationType type) {
        switch (type) {
            case InitializationType::Xavier:
                return initializer::xavier<T>;
            case InitializationType::He:
                return [](std::size_t fanIn, std::size_t fanOut) {
                    return initializer::he<T>(fanIn);
                };
            case InitializationType::Random:
                return [](std::size_t fanIn, std::size_t fanOut) {
                    return initializer::randomUniform<T>();
                };
            case InitializationType::Zero:
                return [](std::size_t fanIn, std::size_t fanOut) {
                    return initializer::zeros<T>();
                };
            case InitializationType::One:
                return [](std::size_t fanIn, std::size_t fanOut) {
                    return initializer::ones<T>();
                };
            default:
                return initializer::xavier<T>;
        }
    }
};

} // namespace core
} // namespace nnv
