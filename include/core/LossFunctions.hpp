/**
 * @file LossFunctions.hpp
 * @brief Loss functions for neural networks
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <vector>
#include <functional>
#include <cmath>
#include <algorithm>

#include "core/Types.hpp"

namespace nnv {
namespace core {

/**
 * @brief Loss functions namespace
 */
namespace loss {

/**
 * @brief Mean Squared Error loss function
 * @tparam T Numeric type
 * @param outputs Network outputs
 * @param targets Target values
 * @return MSE loss
 */
template<typename T>
T meanSquaredError(const std::vector<T>& outputs, const std::vector<T>& targets) {
    if (outputs.size() != targets.size()) {
        return T{0};
    }
    
    T loss = T{0};
    for (std::size_t i = 0; i < outputs.size(); ++i) {
        T diff = outputs[i] - targets[i];
        loss += diff * diff;
    }
    
    return loss / static_cast<T>(outputs.size());
}

/**
 * @brief MSE gradient function
 * @tparam T Numeric type
 * @param outputs Network outputs
 * @param targets Target values
 * @return Gradient vector
 */
template<typename T>
std::vector<T> meanSquaredErrorGradient(const std::vector<T>& outputs, const std::vector<T>& targets) {
    std::vector<T> gradients(outputs.size());
    
    for (std::size_t i = 0; i < outputs.size(); ++i) {
        gradients[i] = T{2} * (outputs[i] - targets[i]) / static_cast<T>(outputs.size());
    }
    
    return gradients;
}

/**
 * @brief Cross-entropy loss function
 * @tparam T Numeric type
 * @param outputs Network outputs (probabilities)
 * @param targets Target values (one-hot encoded)
 * @return Cross-entropy loss
 */
template<typename T>
T crossEntropy(const std::vector<T>& outputs, const std::vector<T>& targets) {
    if (outputs.size() != targets.size()) {
        return T{0};
    }
    
    T loss = T{0};
    const T epsilon = T{1e-15}; // Small value to prevent log(0)
    
    for (std::size_t i = 0; i < outputs.size(); ++i) {
        T clampedOutput = std::max(epsilon, std::min(T{1} - epsilon, outputs[i]));
        loss -= targets[i] * std::log(clampedOutput);
    }
    
    return loss;
}

/**
 * @brief Cross-entropy gradient function
 * @tparam T Numeric type
 * @param outputs Network outputs
 * @param targets Target values
 * @return Gradient vector
 */
template<typename T>
std::vector<T> crossEntropyGradient(const std::vector<T>& outputs, const std::vector<T>& targets) {
    std::vector<T> gradients(outputs.size());
    const T epsilon = T{1e-15};
    
    for (std::size_t i = 0; i < outputs.size(); ++i) {
        T clampedOutput = std::max(epsilon, std::min(T{1} - epsilon, outputs[i]));
        gradients[i] = -targets[i] / clampedOutput;
    }
    
    return gradients;
}

/**
 * @brief Binary cross-entropy loss function
 * @tparam T Numeric type
 * @param outputs Network outputs (single probability)
 * @param targets Target values (0 or 1)
 * @return Binary cross-entropy loss
 */
template<typename T>
T binaryCrossEntropy(const std::vector<T>& outputs, const std::vector<T>& targets) {
    if (outputs.size() != targets.size()) {
        return T{0};
    }
    
    T loss = T{0};
    const T epsilon = T{1e-15};
    
    for (std::size_t i = 0; i < outputs.size(); ++i) {
        T clampedOutput = std::max(epsilon, std::min(T{1} - epsilon, outputs[i]));
        loss -= targets[i] * std::log(clampedOutput) + (T{1} - targets[i]) * std::log(T{1} - clampedOutput);
    }
    
    return loss / static_cast<T>(outputs.size());
}

/**
 * @brief Binary cross-entropy gradient function
 * @tparam T Numeric type
 * @param outputs Network outputs
 * @param targets Target values
 * @return Gradient vector
 */
template<typename T>
std::vector<T> binaryCrossEntropyGradient(const std::vector<T>& outputs, const std::vector<T>& targets) {
    std::vector<T> gradients(outputs.size());
    const T epsilon = T{1e-15};
    
    for (std::size_t i = 0; i < outputs.size(); ++i) {
        T clampedOutput = std::max(epsilon, std::min(T{1} - epsilon, outputs[i]));
        gradients[i] = (clampedOutput - targets[i]) / (clampedOutput * (T{1} - clampedOutput));
        gradients[i] /= static_cast<T>(outputs.size());
    }
    
    return gradients;
}

/**
 * @brief Huber loss function (smooth L1 loss)
 * @tparam T Numeric type
 * @param outputs Network outputs
 * @param targets Target values
 * @param delta Huber delta parameter
 * @return Huber loss
 */
template<typename T>
T huberLoss(const std::vector<T>& outputs, const std::vector<T>& targets, T delta = T{1}) {
    if (outputs.size() != targets.size()) {
        return T{0};
    }
    
    T loss = T{0};
    
    for (std::size_t i = 0; i < outputs.size(); ++i) {
        T diff = std::abs(outputs[i] - targets[i]);
        
        if (diff <= delta) {
            loss += T{0.5} * diff * diff;
        } else {
            loss += delta * diff - T{0.5} * delta * delta;
        }
    }
    
    return loss / static_cast<T>(outputs.size());
}

/**
 * @brief Huber loss gradient function
 * @tparam T Numeric type
 * @param outputs Network outputs
 * @param targets Target values
 * @param delta Huber delta parameter
 * @return Gradient vector
 */
template<typename T>
std::vector<T> huberLossGradient(const std::vector<T>& outputs, const std::vector<T>& targets, T delta = T{1}) {
    std::vector<T> gradients(outputs.size());
    
    for (std::size_t i = 0; i < outputs.size(); ++i) {
        T diff = outputs[i] - targets[i];
        T absDiff = std::abs(diff);
        
        if (absDiff <= delta) {
            gradients[i] = diff;
        } else {
            gradients[i] = delta * (diff > T{0} ? T{1} : T{-1});
        }
        
        gradients[i] /= static_cast<T>(outputs.size());
    }
    
    return gradients;
}

/**
 * @brief Focal loss function (for imbalanced classification)
 * @tparam T Numeric type
 * @param outputs Network outputs (probabilities)
 * @param targets Target values (one-hot encoded)
 * @param alpha Weighting factor for rare class
 * @param gamma Focusing parameter
 * @return Focal loss
 */
template<typename T>
T focalLoss(const std::vector<T>& outputs, const std::vector<T>& targets, T alpha = T{1}, T gamma = T{2}) {
    if (outputs.size() != targets.size()) {
        return T{0};
    }
    
    T loss = T{0};
    const T epsilon = T{1e-15};
    
    for (std::size_t i = 0; i < outputs.size(); ++i) {
        T clampedOutput = std::max(epsilon, std::min(T{1} - epsilon, outputs[i]));
        T pt = targets[i] * clampedOutput + (T{1} - targets[i]) * (T{1} - clampedOutput);
        T logPt = std::log(pt);
        
        loss -= alpha * std::pow(T{1} - pt, gamma) * logPt;
    }
    
    return loss / static_cast<T>(outputs.size());
}

/**
 * @brief Focal loss gradient function
 * @tparam T Numeric type
 * @param outputs Network outputs
 * @param targets Target values
 * @param alpha Weighting factor for rare class
 * @param gamma Focusing parameter
 * @return Gradient vector
 */
template<typename T>
std::vector<T> focalLossGradient(const std::vector<T>& outputs, const std::vector<T>& targets, 
                                T alpha = T{1}, T gamma = T{2}) {
    std::vector<T> gradients(outputs.size());
    const T epsilon = T{1e-15};
    
    for (std::size_t i = 0; i < outputs.size(); ++i) {
        T clampedOutput = std::max(epsilon, std::min(T{1} - epsilon, outputs[i]));
        T pt = targets[i] * clampedOutput + (T{1} - targets[i]) * (T{1} - clampedOutput);
        
        T factor1 = alpha * std::pow(T{1} - pt, gamma);
        T factor2 = gamma * std::pow(T{1} - pt, gamma - T{1}) * std::log(pt);
        
        if (targets[i] == T{1}) {
            gradients[i] = -factor1 / clampedOutput + factor2;
        } else {
            gradients[i] = factor1 / (T{1} - clampedOutput) - factor2;
        }
        
        gradients[i] /= static_cast<T>(outputs.size());
    }
    
    return gradients;
}

} // namespace loss

/**
 * @brief Loss function factory
 */
class LossFactory {
public:
    /**
     * @brief Get loss function by type
     * @tparam T Numeric type
     * @param type Loss function type
     * @return Loss function
     */
    template<typename T>
    static std::function<T(const std::vector<T>&, const std::vector<T>&)> getFunction(LossType type) {
        switch (type) {
            case LossType::MeanSquaredError:
                return loss::meanSquaredError<T>;
            case LossType::CrossEntropy:
                return loss::crossEntropy<T>;
            case LossType::BinaryCrossEntropy:
                return loss::binaryCrossEntropy<T>;
            case LossType::Huber:
                return [](const std::vector<T>& outputs, const std::vector<T>& targets) {
                    return loss::huberLoss(outputs, targets);
                };
            case LossType::FocalLoss:
                return [](const std::vector<T>& outputs, const std::vector<T>& targets) {
                    return loss::focalLoss(outputs, targets);
                };
            default:
                return loss::meanSquaredError<T>;
        }
    }
    
    /**
     * @brief Get loss gradient function by type
     * @tparam T Numeric type
     * @param type Loss function type
     * @return Loss gradient function
     */
    template<typename T>
    static std::function<std::vector<T>(const std::vector<T>&, const std::vector<T>&)> 
    getGradient(LossType type) {
        switch (type) {
            case LossType::MeanSquaredError:
                return loss::meanSquaredErrorGradient<T>;
            case LossType::CrossEntropy:
                return loss::crossEntropyGradient<T>;
            case LossType::BinaryCrossEntropy:
                return loss::binaryCrossEntropyGradient<T>;
            case LossType::Huber:
                return [](const std::vector<T>& outputs, const std::vector<T>& targets) {
                    return loss::huberLossGradient(outputs, targets);
                };
            case LossType::FocalLoss:
                return [](const std::vector<T>& outputs, const std::vector<T>& targets) {
                    return loss::focalLossGradient(outputs, targets);
                };
            default:
                return loss::meanSquaredErrorGradient<T>;
        }
    }
};

} // namespace core
} // namespace nnv
