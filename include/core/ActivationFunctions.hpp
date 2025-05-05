/**
 * @file ActivationFunctions.hpp
 * @brief Activation functions for neural networks
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <functional>
#include <cmath>
#include <algorithm>
#include <vector>

#include "core/Types.hpp"

namespace nnv {
namespace core {

/**
 * @brief Activation functions namespace
 */
namespace activation {

/**
 * @brief Linear activation (identity function)
 * @tparam T Numeric type
 * @param x Input value
 * @return x
 */
template<typename T>
inline T linear(T x) {
    return x;
}

/**
 * @brief Linear activation derivative
 * @tparam T Numeric type
 * @param x Input value
 * @return 1
 */
template<typename T>
inline T linearDerivative(T x) {
    (void)x; // Suppress unused parameter warning
    return T{1};
}

/**
 * @brief ReLU activation function
 * @tparam T Numeric type
 * @param x Input value
 * @return max(0, x)
 */
template<typename T>
inline T relu(T x) {
    return std::max(T{0}, x);
}

/**
 * @brief ReLU derivative
 * @tparam T Numeric type
 * @param x Input value
 * @return 1 if x > 0, else 0
 */
template<typename T>
inline T reluDerivative(T x) {
    return x > T{0} ? T{1} : T{0};
}

/**
 * @brief Leaky ReLU activation function
 * @tparam T Numeric type
 * @param x Input value
 * @param alpha Leak coefficient (default 0.01)
 * @return max(alpha * x, x)
 */
template<typename T>
inline T leakyRelu(T x, T alpha = T{0.01}) {
    return x > T{0} ? x : alpha * x;
}

/**
 * @brief Leaky ReLU derivative
 * @tparam T Numeric type
 * @param x Input value
 * @param alpha Leak coefficient (default 0.01)
 * @return 1 if x > 0, else alpha
 */
template<typename T>
inline T leakyReluDerivative(T x, T alpha = T{0.01}) {
    return x > T{0} ? T{1} : alpha;
}

/**
 * @brief Sigmoid activation function
 * @tparam T Numeric type
 * @param x Input value
 * @return 1 / (1 + exp(-x))
 */
template<typename T>
inline T sigmoid(T x) {
    // Clamp x to prevent overflow
    x = std::max(T{-500}, std::min(T{500}, x));
    return T{1} / (T{1} + std::exp(-x));
}

/**
 * @brief Sigmoid derivative
 * @tparam T Numeric type
 * @param x Input value
 * @return sigmoid(x) * (1 - sigmoid(x))
 */
template<typename T>
inline T sigmoidDerivative(T x) {
    T s = sigmoid(x);
    return s * (T{1} - s);
}

/**
 * @brief Hyperbolic tangent activation function
 * @tparam T Numeric type
 * @param x Input value
 * @return tanh(x)
 */
template<typename T>
inline T tanh(T x) {
    return std::tanh(x);
}

/**
 * @brief Hyperbolic tangent derivative
 * @tparam T Numeric type
 * @param x Input value
 * @return 1 - tanh²(x)
 */
template<typename T>
inline T tanhDerivative(T x) {
    T t = std::tanh(x);
    return T{1} - t * t;
}

/**
 * @brief ELU (Exponential Linear Unit) activation function
 * @tparam T Numeric type
 * @param x Input value
 * @param alpha Alpha parameter (default 1.0)
 * @return x if x > 0, else alpha * (exp(x) - 1)
 */
template<typename T>
inline T elu(T x, T alpha = T{1}) {
    return x > T{0} ? x : alpha * (std::exp(x) - T{1});
}

/**
 * @brief ELU derivative
 * @tparam T Numeric type
 * @param x Input value
 * @param alpha Alpha parameter (default 1.0)
 * @return 1 if x > 0, else alpha * exp(x)
 */
template<typename T>
inline T eluDerivative(T x, T alpha = T{1}) {
    return x > T{0} ? T{1} : alpha * std::exp(x);
}

/**
 * @brief Swish activation function
 * @tparam T Numeric type
 * @param x Input value
 * @return x * sigmoid(x)
 */
template<typename T>
inline T swish(T x) {
    return x * sigmoid(x);
}

/**
 * @brief Swish derivative
 * @tparam T Numeric type
 * @param x Input value
 * @return swish(x) + sigmoid(x) * (1 - swish(x))
 */
template<typename T>
inline T swishDerivative(T x) {
    T s = sigmoid(x);
    T sw = x * s;
    return sw + s * (T{1} - sw);
}

/**
 * @brief GELU (Gaussian Error Linear Unit) activation function
 * @tparam T Numeric type
 * @param x Input value
 * @return 0.5 * x * (1 + tanh(sqrt(2/π) * (x + 0.044715 * x³)))
 */
template<typename T>
inline T gelu(T x) {
    const T sqrt_2_over_pi = T{0.7978845608028654}; // sqrt(2/π)
    const T coeff = T{0.044715};
    T x_cubed = x * x * x;
    return T{0.5} * x * (T{1} + std::tanh(sqrt_2_over_pi * (x + coeff * x_cubed)));
}

/**
 * @brief GELU derivative (approximation)
 * @tparam T Numeric type
 * @param x Input value
 * @return Approximate derivative of GELU
 */
template<typename T>
inline T geluDerivative(T x) {
    const T sqrt_2_over_pi = T{0.7978845608028654};
    const T coeff = T{0.044715};
    T x_squared = x * x;
    T x_cubed = x_squared * x;
    T inner = sqrt_2_over_pi * (x + coeff * x_cubed);
    T tanh_inner = std::tanh(inner);
    T sech_squared = T{1} - tanh_inner * tanh_inner;
    
    return T{0.5} * (T{1} + tanh_inner) + 
           T{0.5} * x * sech_squared * sqrt_2_over_pi * (T{1} + T{3} * coeff * x_squared);
}

/**
 * @brief Softmax activation function for a vector
 * @tparam T Numeric type
 * @param x Input vector
 * @return Softmax output vector
 */
template<typename T>
std::vector<T> softmax(const std::vector<T>& x) {
    std::vector<T> result(x.size());
    
    // Find maximum for numerical stability
    T max_val = *std::max_element(x.begin(), x.end());
    
    // Compute exponentials and sum
    T sum = T{0};
    for (std::size_t i = 0; i < x.size(); ++i) {
        result[i] = std::exp(x[i] - max_val);
        sum += result[i];
    }
    
    // Normalize
    for (std::size_t i = 0; i < x.size(); ++i) {
        result[i] /= sum;
    }
    
    return result;
}

/**
 * @brief Softmax derivative for a vector
 * @tparam T Numeric type
 * @param x Input vector
 * @param i Index for which to compute derivative
 * @param j Index of the output
 * @return Derivative of softmax[i] with respect to x[j]
 */
template<typename T>
T softmaxDerivative(const std::vector<T>& x, std::size_t i, std::size_t j) {
    auto sm = softmax(x);
    if (i == j) {
        return sm[i] * (T{1} - sm[i]);
    } else {
        return -sm[i] * sm[j];
    }
}

} // namespace activation

/**
 * @brief Activation function factory
 */
class ActivationFactory {
public:
    /**
     * @brief Get activation function by type
     * @tparam T Numeric type
     * @param type Activation type
     * @return Activation function
     */
    template<typename T>
    static std::function<T(T)> getFunction(ActivationType type) {
        switch (type) {
            case ActivationType::None:
                return activation::linear<T>;
            case ActivationType::ReLU:
                return activation::relu<T>;
            case ActivationType::Sigmoid:
                return activation::sigmoid<T>;
            case ActivationType::Tanh:
                return activation::tanh<T>;
            case ActivationType::LeakyReLU:
                return [](T x) { return activation::leakyRelu(x); };
            case ActivationType::ELU:
                return [](T x) { return activation::elu(x); };
            case ActivationType::Swish:
                return activation::swish<T>;
            case ActivationType::GELU:
                return activation::gelu<T>;
            case ActivationType::Softmax:
                // Note: Softmax is handled differently as it operates on vectors
                return activation::linear<T>;
            default:
                return activation::relu<T>;
        }
    }
    
    /**
     * @brief Get activation derivative function by type
     * @tparam T Numeric type
     * @param type Activation type
     * @return Activation derivative function
     */
    template<typename T>
    static std::function<T(T)> getDerivative(ActivationType type) {
        switch (type) {
            case ActivationType::None:
                return activation::linearDerivative<T>;
            case ActivationType::ReLU:
                return activation::reluDerivative<T>;
            case ActivationType::Sigmoid:
                return activation::sigmoidDerivative<T>;
            case ActivationType::Tanh:
                return activation::tanhDerivative<T>;
            case ActivationType::LeakyReLU:
                return [](T x) { return activation::leakyReluDerivative(x); };
            case ActivationType::ELU:
                return [](T x) { return activation::eluDerivative(x); };
            case ActivationType::Swish:
                return activation::swishDerivative<T>;
            case ActivationType::GELU:
                return activation::geluDerivative<T>;
            case ActivationType::Softmax:
                return activation::linearDerivative<T>;
            default:
                return activation::reluDerivative<T>;
        }
    }
};

} // namespace core
} // namespace nnv
