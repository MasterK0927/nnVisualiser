/**
 * @file NeuralNetwork.hpp
 * @brief Main neural network class for the Neural Network Visualizer
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>

#include "core/Types.hpp"
#include "core/Layer.hpp"
#include "utils/Common.hpp"

namespace nnv {
namespace core {

/**
 * @brief Main neural network class with training and inference capabilities
 * @tparam T Numeric type (float, double)
 */
template<typename T = Scalar>
class NeuralNetwork {
public:
    /**
     * @brief Constructor
     * @param name Network name
     */
    explicit NeuralNetwork(const std::string& name = "Neural Network");
    
    /**
     * @brief Constructor from configuration
     * @param config Network configuration
     */
    explicit NeuralNetwork(const NetworkConfig& config);
    
    /**
     * @brief Destructor
     */
    ~NeuralNetwork();
    
    // Disable copy constructor and assignment
    NNV_DISABLE_COPY(NeuralNetwork)
    
    // Enable move constructor and assignment
    NeuralNetwork(NeuralNetwork&& other) noexcept = default;
    NeuralNetwork& operator=(NeuralNetwork&& other) noexcept = default;
    
    /**
     * @brief Get network name
     * @return Network name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief Set network name
     * @param name New network name
     */
    void setName(const std::string& name) { name_ = name; }
    
    /**
     * @brief Get number of layers
     * @return Number of layers
     */
    std::size_t getLayerCount() const { return layers_.size(); }
    
    /**
     * @brief Get layer by index
     * @param index Layer index
     * @return Reference to layer
     */
    Layer<T>& getLayer(LayerIndex index) {
        NNV_ASSERT(index < layers_.size());
        return *layers_[index];
    }
    
    /**
     * @brief Get layer by index (const version)
     * @param index Layer index
     * @return Const reference to layer
     */
    const Layer<T>& getLayer(LayerIndex index) const {
        NNV_ASSERT(index < layers_.size());
        return *layers_[index];
    }
    
    /**
     * @brief Add a layer to the network
     * @param layer Layer to add
     */
    void addLayer(std::unique_ptr<Layer<T>> layer);
    
    /**
     * @brief Add a layer with configuration
     * @param config Layer configuration
     */
    void addLayer(const LayerConfig& config);
    
    /**
     * @brief Remove layer by index
     * @param index Layer index to remove
     */
    void removeLayer(LayerIndex index);
    
    /**
     * @brief Clear all layers
     */
    void clearLayers();
    
    /**
     * @brief Initialize network weights
     * @param initType Weight initialization type
     */
    void initializeWeights(InitializationType initType = InitializationType::Xavier);
    
    /**
     * @brief Forward pass through the network
     * @param inputs Input vector
     * @return Output vector
     */
    std::vector<T> forward(const std::vector<T>& inputs);
    
    /**
     * @brief Backward pass (backpropagation)
     * @param targets Target values
     * @param outputs Network outputs
     * @return Loss value
     */
    T backward(const std::vector<T>& targets, const std::vector<T>& outputs);
    
    /**
     * @brief Train the network on a single sample
     * @param inputs Input vector
     * @param targets Target vector
     * @return Loss value
     */
    T trainSample(const std::vector<T>& inputs, const std::vector<T>& targets);
    
    /**
     * @brief Train the network on a batch of samples
     * @param inputBatch Batch of input vectors
     * @param targetBatch Batch of target vectors
     * @return Average loss
     */
    T trainBatch(const std::vector<std::vector<T>>& inputBatch,
                 const std::vector<std::vector<T>>& targetBatch);
    
    /**
     * @brief Train the network for multiple epochs
     * @param inputData Training input data
     * @param targetData Training target data
     * @param epochs Number of epochs
     * @param batchSize Batch size
     * @param validationData Optional validation data
     * @param progressCallback Optional progress callback
     * @return Training history
     */
    struct TrainingHistory {
        std::vector<T> trainLoss;
        std::vector<T> trainAccuracy;
        std::vector<T> valLoss;
        std::vector<T> valAccuracy;
    };
    
    TrainingHistory train(const std::vector<std::vector<T>>& inputData,
                         const std::vector<std::vector<T>>& targetData,
                         std::size_t epochs,
                         std::size_t batchSize = 32,
                         const std::vector<std::vector<T>>* validationInputs = nullptr,
                         const std::vector<std::vector<T>>* validationTargets = nullptr,
                         ProgressCallback progressCallback = nullptr);
    
    /**
     * @brief Evaluate the network on test data
     * @param inputData Test input data
     * @param targetData Test target data
     * @return Evaluation metrics (loss, accuracy)
     */
    std::pair<T, T> evaluate(const std::vector<std::vector<T>>& inputData,
                            const std::vector<std::vector<T>>& targetData);
    
    /**
     * @brief Predict outputs for given inputs
     * @param inputs Input vector
     * @return Predicted outputs
     */
    std::vector<T> predict(const std::vector<T>& inputs);
    
    /**
     * @brief Predict outputs for batch of inputs
     * @param inputBatch Batch of input vectors
     * @return Batch of predicted outputs
     */
    std::vector<std::vector<T>> predictBatch(const std::vector<std::vector<T>>& inputBatch);
    
    /**
     * @brief Set learning rate
     * @param learningRate New learning rate
     */
    void setLearningRate(T learningRate) { learningRate_ = learningRate; }
    
    /**
     * @brief Get learning rate
     * @return Current learning rate
     */
    T getLearningRate() const { return learningRate_; }
    
    /**
     * @brief Set loss function type
     * @param lossType New loss function type
     */
    void setLossType(LossType lossType);
    
    /**
     * @brief Get loss function type
     * @return Current loss function type
     */
    LossType getLossType() const { return lossType_; }
    
    /**
     * @brief Set optimizer type
     * @param optimizerType New optimizer type
     */
    void setOptimizerType(OptimizerType optimizerType);
    
    /**
     * @brief Get optimizer type
     * @return Current optimizer type
     */
    OptimizerType getOptimizerType() const { return optimizerType_; }
    
    /**
     * @brief Reset network state
     */
    void reset();
    
    /**
     * @brief Check if network is currently training
     * @return True if training
     */
    bool isTraining() const { return isTraining_.load(); }
    
    /**
     * @brief Stop training (for async training)
     */
    void stopTraining() { shouldStop_.store(true); }
    
    /**
     * @brief Get training progress (0.0 to 1.0)
     * @return Training progress
     */
    T getTrainingProgress() const { return trainingProgress_.load(); }
    
    /**
     * @brief Serialize network to JSON
     * @return JSON representation
     */
    nlohmann::json toJson() const;
    
    /**
     * @brief Deserialize network from JSON
     * @param json JSON representation
     */
    void fromJson(const nlohmann::json& json);
    
    /**
     * @brief Save network to file
     * @param filename File path
     * @return True if successful
     */
    bool saveToFile(const std::string& filename) const;
    
    /**
     * @brief Load network from file
     * @param filename File path
     * @return True if successful
     */
    bool loadFromFile(const std::string& filename);

private:
    std::string name_;                              ///< Network name
    std::vector<std::unique_ptr<Layer<T>>> layers_; ///< Network layers
    T learningRate_;                               ///< Learning rate
    LossType lossType_;                           ///< Loss function type
    OptimizerType optimizerType_;                 ///< Optimizer type
    
    // Training state
    std::atomic<bool> isTraining_;                ///< Training flag
    std::atomic<bool> shouldStop_;                ///< Stop training flag
    std::atomic<T> trainingProgress_;             ///< Training progress
    mutable std::mutex networkMutex_;             ///< Thread safety
    
    // Loss and optimizer functions
    std::function<T(const std::vector<T>&, const std::vector<T>&)> lossFunction_;
    std::function<std::vector<T>(const std::vector<T>&, const std::vector<T>&)> lossGradientFunction_;
    
    /**
     * @brief Update loss function based on type
     */
    void updateLossFunction();
    
    /**
     * @brief Update optimizer based on type
     */
    void updateOptimizer();
    
    /**
     * @brief Compute accuracy for classification tasks
     * @param outputs Network outputs
     * @param targets Target values
     * @return Accuracy (0.0 to 1.0)
     */
    T computeAccuracy(const std::vector<std::vector<T>>& outputs,
                     const std::vector<std::vector<T>>& targets) const;
    
    /**
     * @brief Shuffle training data
     * @param inputs Input data
     * @param targets Target data
     */
    void shuffleData(std::vector<std::vector<T>>& inputs,
                    std::vector<std::vector<T>>& targets) const;
    
    /**
     * @brief Create mini-batches from data
     * @param inputs Input data
     * @param targets Target data
     * @param batchSize Batch size
     * @return Vector of batches
     */
    std::vector<std::pair<std::vector<std::vector<T>>, std::vector<std::vector<T>>>>
    createBatches(const std::vector<std::vector<T>>& inputs,
                 const std::vector<std::vector<T>>& targets,
                 std::size_t batchSize) const;
};

// Type aliases
using FloatNetwork = NeuralNetwork<float>;
using DoubleNetwork = NeuralNetwork<double>;

} // namespace core
} // namespace nnv
