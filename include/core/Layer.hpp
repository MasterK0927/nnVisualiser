/**
 * @file Layer.hpp
 * @brief Layer class for neural networks
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>

#include "core/Types.hpp"
#include "core/Neuron.hpp"
#include "core/ActivationFunctions.hpp"
#include "utils/Common.hpp"

namespace nnv {
namespace core {

/**
 * @brief Neural network layer class
 * @tparam T Numeric type (float, double)
 */
template<typename T = Scalar>
class Layer {
public:
    /**
     * @brief Constructor
     * @param size Number of neurons in this layer
     * @param activation Activation function type
     * @param name Layer name
     */
    explicit Layer(LayerSize size, 
                   ActivationType activation = ActivationType::ReLU,
                   const std::string& name = "");
    
    /**
     * @brief Constructor from configuration
     * @param config Layer configuration
     */
    explicit Layer(const LayerConfig& config);
    
    /**
     * @brief Destructor
     */
    ~Layer() = default;
    
    // Disable copy constructor and assignment
    NNV_DISABLE_COPY(Layer)
    
    // Enable move constructor and assignment
    Layer(Layer&& other) noexcept = default;
    Layer& operator=(Layer&& other) noexcept = default;
    
    /**
     * @brief Get layer size (number of neurons)
     * @return Number of neurons
     */
    LayerSize getSize() const { return neurons_.size(); }
    
    /**
     * @brief Get layer name
     * @return Layer name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief Set layer name
     * @param name New layer name
     */
    void setName(const std::string& name) { name_ = name; }
    
    /**
     * @brief Get activation type
     * @return Activation function type
     */
    ActivationType getActivationType() const { return activationType_; }
    
    /**
     * @brief Set activation type
     * @param type New activation type
     */
    void setActivationType(ActivationType type);
    
    /**
     * @brief Get dropout rate
     * @return Dropout rate (0.0 to 1.0)
     */
    T getDropoutRate() const { return dropoutRate_; }
    
    /**
     * @brief Set dropout rate
     * @param rate New dropout rate (0.0 to 1.0)
     */
    void setDropoutRate(T rate) { dropoutRate_ = std::max(T{0}, std::min(T{1}, rate)); }
    
    /**
     * @brief Check if layer is trainable
     * @return True if trainable
     */
    bool isTrainable() const { return trainable_; }
    
    /**
     * @brief Set trainable flag
     * @param trainable New trainable state
     */
    void setTrainable(bool trainable) { trainable_ = trainable; }
    
    /**
     * @brief Get neuron by index
     * @param index Neuron index
     * @return Reference to neuron
     */
    Neuron<T>& getNeuron(NeuronIndex index) {
        NNV_ASSERT(index < neurons_.size());
        return neurons_[index];
    }
    
    /**
     * @brief Get neuron by index (const version)
     * @param index Neuron index
     * @return Const reference to neuron
     */
    const Neuron<T>& getNeuron(NeuronIndex index) const {
        NNV_ASSERT(index < neurons_.size());
        return neurons_[index];
    }
    
    /**
     * @brief Get all neurons
     * @return Vector of neurons
     */
    std::vector<Neuron<T>>& getNeurons() { return neurons_; }
    const std::vector<Neuron<T>>& getNeurons() const { return neurons_; }
    
    /**
     * @brief Get activations of all neurons
     * @return Vector of activation values
     */
    std::vector<T> getActivations() const;
    
    /**
     * @brief Set activations of all neurons
     * @param activations Vector of activation values
     */
    void setActivations(const std::vector<T>& activations);
    
    /**
     * @brief Get biases of all neurons
     * @return Vector of bias values
     */
    std::vector<T> getBiases() const;
    
    /**
     * @brief Set biases of all neurons
     * @param biases Vector of bias values
     */
    void setBiases(const std::vector<T>& biases);
    
    /**
     * @brief Initialize weights connecting to previous layer
     * @param prevLayerSize Size of previous layer
     * @param initType Weight initialization type
     */
    void initializeWeights(LayerSize prevLayerSize, InitializationType initType = InitializationType::Xavier);
    
    /**
     * @brief Forward pass computation
     * @param inputs Input values from previous layer
     */
    void forward(const std::vector<T>& inputs);
    
    /**
     * @brief Apply activation function to all neurons
     */
    void applyActivation();
    
    /**
     * @brief Apply dropout during training
     * @param training Whether in training mode
     */
    void applyDropout(bool training = true);
    
    /**
     * @brief Compute gradients for backpropagation
     * @param nextLayerDeltas Deltas from next layer
     * @param nextLayerWeights Weights from next layer
     */
    void computeGradients(const std::vector<T>& nextLayerDeltas,
                         const std::vector<std::vector<T>>& nextLayerWeights);
    
    /**
     * @brief Update weights using computed gradients
     * @param learningRate Learning rate for updates
     * @param prevLayerActivations Activations from previous layer
     */
    void updateWeights(T learningRate, const std::vector<T>& prevLayerActivations);
    
    /**
     * @brief Reset layer state
     */
    void reset();
    
    /**
     * @brief Get weight matrix (connections from previous layer)
     * @return 2D weight matrix
     */
    std::vector<std::vector<T>> getWeightMatrix() const;
    
    /**
     * @brief Set weight matrix
     * @param weights 2D weight matrix
     */
    void setWeightMatrix(const std::vector<std::vector<T>>& weights);
    
    /**
     * @brief Serialize layer to JSON
     * @return JSON representation
     */
    nlohmann::json toJson() const;
    
    /**
     * @brief Deserialize layer from JSON
     * @param json JSON representation
     */
    void fromJson(const nlohmann::json& json);

private:
    std::vector<Neuron<T>> neurons_;        ///< Neurons in this layer
    std::string name_;                      ///< Layer name
    ActivationType activationType_;         ///< Activation function type
    T dropoutRate_;                        ///< Dropout rate (0.0 to 1.0)
    bool trainable_;                       ///< Whether layer is trainable
    
    // Activation functions
    std::function<T(T)> activationFunc_;
    std::function<T(T)> activationDerivFunc_;
    
    // Dropout mask for training
    std::vector<bool> dropoutMask_;
    
    /**
     * @brief Update activation functions based on type
     */
    void updateActivationFunctions();
    
    /**
     * @brief Initialize neuron IDs
     */
    void initializeNeuronIds();
    
    /**
     * @brief Apply Xavier/Glorot initialization
     * @param prevLayerSize Size of previous layer
     */
    void initializeXavier(LayerSize prevLayerSize);
    
    /**
     * @brief Apply He initialization
     * @param prevLayerSize Size of previous layer
     */
    void initializeHe(LayerSize prevLayerSize);
    
    /**
     * @brief Apply random initialization
     * @param prevLayerSize Size of previous layer
     */
    void initializeRandom(LayerSize prevLayerSize);
};

// Type aliases
using FloatLayer = Layer<float>;
using DoubleLayer = Layer<double>;

} // namespace core
} // namespace nnv
