/**
 * @file Neuron.hpp
 * @brief Neuron class for the Neural Network Visualizer
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <vector>
#include <memory>
#include <functional>

#include "core/Types.hpp"
#include "utils/Common.hpp"

namespace nnv {
namespace core {

/**
 * @brief Individual neuron class with configurable properties
 * @tparam T Numeric type (float, double)
 */
template<typename T = Scalar>
class Neuron {
public:
    /**
     * @brief Constructor
     * @param id Unique identifier for this neuron
     */
    explicit Neuron(NeuronIndex id = 0);
    
    /**
     * @brief Destructor
     */
    ~Neuron() = default;
    
    // Copy and move constructors
    Neuron(const Neuron& other) = default;
    Neuron& operator=(const Neuron& other) = default;
    Neuron(Neuron&& other) noexcept = default;
    Neuron& operator=(Neuron&& other) noexcept = default;
    
    /**
     * @brief Get neuron ID
     * @return Neuron identifier
     */
    NeuronIndex getId() const { return id_; }
    
    /**
     * @brief Set neuron ID
     * @param id New identifier
     */
    void setId(NeuronIndex id) { id_ = id; }
    
    /**
     * @brief Get current activation value
     * @return Current activation
     */
    T getActivation() const { return activation_; }
    
    /**
     * @brief Set activation value
     * @param activation New activation value
     */
    void setActivation(T activation) { activation_ = activation; }
    
    /**
     * @brief Get bias value
     * @return Current bias
     */
    T getBias() const { return bias_; }
    
    /**
     * @brief Set bias value
     * @param bias New bias value
     */
    void setBias(T bias) { bias_ = bias; }
    
    /**
     * @brief Get weighted input (before activation)
     * @return Weighted input value
     */
    T getWeightedInput() const { return weightedInput_; }
    
    /**
     * @brief Set weighted input
     * @param input New weighted input value
     */
    void setWeightedInput(T input) { weightedInput_ = input; }
    
    /**
     * @brief Get gradient for backpropagation
     * @return Current gradient
     */
    T getGradient() const { return gradient_; }
    
    /**
     * @brief Set gradient for backpropagation
     * @param gradient New gradient value
     */
    void setGradient(T gradient) { gradient_ = gradient; }
    
    /**
     * @brief Get error delta for backpropagation
     * @return Current error delta
     */
    T getDelta() const { return delta_; }
    
    /**
     * @brief Set error delta for backpropagation
     * @param delta New error delta
     */
    void setDelta(T delta) { delta_ = delta; }
    
    /**
     * @brief Check if neuron is trainable
     * @return True if trainable
     */
    bool isTrainable() const { return trainable_; }
    
    /**
     * @brief Set trainable flag
     * @param trainable New trainable state
     */
    void setTrainable(bool trainable) { trainable_ = trainable; }
    
    /**
     * @brief Get neuron name/label
     * @return Neuron name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief Set neuron name/label
     * @param name New name
     */
    void setName(const std::string& name) { name_ = name; }
    
    /**
     * @brief Reset neuron state
     */
    void reset();
    
    /**
     * @brief Apply activation function to weighted input
     * @param activationFunc Activation function to apply
     */
    void applyActivation(const std::function<T(T)>& activationFunc);
    
    /**
     * @brief Compute activation derivative for backpropagation
     * @param derivativeFunc Activation derivative function
     * @return Activation derivative
     */
    T computeActivationDerivative(const std::function<T(T)>& derivativeFunc) const;
    
    /**
     * @brief Add input connection weight
     * @param weight Connection weight
     */
    void addInputWeight(T weight) { inputWeights_.push_back(weight); }
    
    /**
     * @brief Get input weights
     * @return Vector of input weights
     */
    const std::vector<T>& getInputWeights() const { return inputWeights_; }
    
    /**
     * @brief Set input weights
     * @param weights New input weights
     */
    void setInputWeights(const std::vector<T>& weights) { inputWeights_ = weights; }
    
    /**
     * @brief Get specific input weight
     * @param index Weight index
     * @return Weight value
     */
    T getInputWeight(std::size_t index) const {
        return index < inputWeights_.size() ? inputWeights_[index] : T{0};
    }
    
    /**
     * @brief Set specific input weight
     * @param index Weight index
     * @param weight New weight value
     */
    void setInputWeight(std::size_t index, T weight) {
        if (index < inputWeights_.size()) {
            inputWeights_[index] = weight;
        }
    }
    
    /**
     * @brief Get number of input connections
     * @return Number of inputs
     */
    std::size_t getInputCount() const { return inputWeights_.size(); }
    
    /**
     * @brief Clear all input weights
     */
    void clearInputWeights() { inputWeights_.clear(); }
    
    /**
     * @brief Serialize neuron to JSON
     * @return JSON representation
     */
    nlohmann::json toJson() const;
    
    /**
     * @brief Deserialize neuron from JSON
     * @param json JSON representation
     */
    void fromJson(const nlohmann::json& json);

private:
    NeuronIndex id_;                    ///< Unique neuron identifier
    T activation_;                      ///< Current activation value
    T bias_;                           ///< Bias value
    T weightedInput_;                  ///< Weighted input (before activation)
    T gradient_;                       ///< Gradient for backpropagation
    T delta_;                          ///< Error delta for backpropagation
    bool trainable_;                   ///< Whether this neuron is trainable
    std::string name_;                 ///< Neuron name/label
    std::vector<T> inputWeights_;      ///< Input connection weights
};

// Type aliases
using FloatNeuron = Neuron<float>;
using DoubleNeuron = Neuron<double>;

} // namespace core
} // namespace nnv
