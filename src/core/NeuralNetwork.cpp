/**
 * @file NeuralNetwork.cpp
 * @brief Implementation of the NeuralNetwork class
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include "core/NeuralNetwork.hpp"
#include "core/LossFunctions.hpp"
#include "utils/Logger.hpp"
#include <algorithm>
#include <random>
#include <fstream>
#include <numeric>

namespace nnv {
namespace core {

template<typename T>
NeuralNetwork<T>::NeuralNetwork(const std::string& name)
    : name_(name)
    , learningRate_(T{0.001})
    , lossType_(LossType::MeanSquaredError)
    , optimizerType_(OptimizerType::Adam)
    , isTraining_(false)
    , shouldStop_(false)
    , trainingProgress_(T{0})
{
    updateLossFunction();
    updateOptimizer();
}

template<typename T>
NeuralNetwork<T>::NeuralNetwork(const NetworkConfig& config)
    : name_(config.name)
    , learningRate_(config.training.learning_rate)
    , lossType_(config.loss)
    , optimizerType_(config.optimizer)
    , isTraining_(false)
    , shouldStop_(false)
    , trainingProgress_(T{0})
{
    // Add layers from configuration
    for (const auto& layerConfig : config.layers) {
        addLayer(layerConfig);
    }
    
    updateLossFunction();
    updateOptimizer();
    
    // Initialize weights after all layers are added
    if (!layers_.empty()) {
        initializeWeights();
    }
}

template<typename T>
NeuralNetwork<T>::~NeuralNetwork() {
    stopTraining();
    // Wait for training to stop if it's running
    while (isTraining_.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

template<typename T>
void NeuralNetwork<T>::addLayer(std::unique_ptr<Layer<T>> layer) {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    if (!layers_.empty()) {
        // Initialize weights for the new layer based on previous layer size
        layer->initializeWeights(layers_.back()->getSize());
    }
    
    layers_.push_back(std::move(layer));
    NNV_LOG_DEBUG("Added layer to network '{}'. Total layers: {}", name_, layers_.size());
}

template<typename T>
void NeuralNetwork<T>::addLayer(const LayerConfig& config) {
    auto layer = std::make_unique<Layer<T>>(config);
    addLayer(std::move(layer));
}

template<typename T>
void NeuralNetwork<T>::removeLayer(LayerIndex index) {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    if (index >= layers_.size()) {
        NNV_LOG_WARNING("Attempted to remove layer {} from network with {} layers", 
                       index, layers_.size());
        return;
    }
    
    layers_.erase(layers_.begin() + index);
    
    // Re-initialize weights for layers after the removed one
    for (std::size_t i = index; i < layers_.size(); ++i) {
        if (i > 0) {
            layers_[i]->initializeWeights(layers_[i-1]->getSize());
        }
    }
    
    NNV_LOG_DEBUG("Removed layer {} from network '{}'. Total layers: {}", 
                 index, name_, layers_.size());
}

template<typename T>
void NeuralNetwork<T>::clearLayers() {
    std::lock_guard<std::mutex> lock(networkMutex_);
    layers_.clear();
    NNV_LOG_DEBUG("Cleared all layers from network '{}'", name_);
}

template<typename T>
void NeuralNetwork<T>::initializeWeights(InitializationType initType) {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    for (std::size_t i = 1; i < layers_.size(); ++i) {
        layers_[i]->initializeWeights(layers_[i-1]->getSize(), initType);
    }
    
    NNV_LOG_DEBUG("Initialized weights for network '{}' using {} initialization", 
                 name_, static_cast<int>(initType));
}

template<typename T>
std::vector<T> NeuralNetwork<T>::forward(const std::vector<T>& inputs) {
    if (layers_.empty()) {
        NNV_LOG_ERROR("Cannot perform forward pass on empty network");
        return {};
    }
    
    if (inputs.size() != layers_[0]->getSize()) {
        NNV_LOG_ERROR("Input size {} doesn't match first layer size {}", 
                     inputs.size(), layers_[0]->getSize());
        return {};
    }
    
    // Set input layer activations
    layers_[0]->setActivations(inputs);
    
    // Forward pass through hidden and output layers
    for (std::size_t i = 1; i < layers_.size(); ++i) {
        auto prevActivations = layers_[i-1]->getActivations();
        layers_[i]->forward(prevActivations);
        layers_[i]->applyActivation();
        layers_[i]->applyDropout(isTraining_.load());
    }
    
    return layers_.back()->getActivations();
}

template<typename T>
T NeuralNetwork<T>::backward(const std::vector<T>& targets, const std::vector<T>& outputs) {
    if (layers_.size() < 2) {
        NNV_LOG_ERROR("Cannot perform backward pass on network with less than 2 layers");
        return T{0};
    }
    
    // Compute loss
    T loss = lossFunction_(outputs, targets);
    
    // Compute output layer gradients
    auto outputGradients = lossGradientFunction_(outputs, targets);
    auto& outputLayer = *layers_.back();
    
    for (std::size_t i = 0; i < outputLayer.getSize(); ++i) {
        outputLayer.getNeuron(i).setDelta(outputGradients[i]);
    }
    
    // Backward pass through hidden layers
    for (int i = static_cast<int>(layers_.size()) - 2; i >= 1; --i) {
        auto& currentLayer = *layers_[i];
        auto& nextLayer = *layers_[i + 1];
        
        // Get deltas and weights from next layer
        std::vector<T> nextLayerDeltas;
        std::vector<std::vector<T>> nextLayerWeights;
        
        for (std::size_t j = 0; j < nextLayer.getSize(); ++j) {
            nextLayerDeltas.push_back(nextLayer.getNeuron(j).getDelta());
            nextLayerWeights.push_back(nextLayer.getNeuron(j).getInputWeights());
        }
        
        currentLayer.computeGradients(nextLayerDeltas, nextLayerWeights);
    }
    
    // Update weights
    for (std::size_t i = 1; i < layers_.size(); ++i) {
        auto prevActivations = layers_[i-1]->getActivations();
        layers_[i]->updateWeights(learningRate_, prevActivations);
    }
    
    return loss;
}

template<typename T>
T NeuralNetwork<T>::trainSample(const std::vector<T>& inputs, const std::vector<T>& targets) {
    auto outputs = forward(inputs);
    return backward(targets, outputs);
}

template<typename T>
T NeuralNetwork<T>::trainBatch(const std::vector<std::vector<T>>& inputBatch,
                              const std::vector<std::vector<T>>& targetBatch) {
    if (inputBatch.size() != targetBatch.size()) {
        NNV_LOG_ERROR("Input batch size {} doesn't match target batch size {}", 
                     inputBatch.size(), targetBatch.size());
        return T{0};
    }
    
    T totalLoss = T{0};
    
    for (std::size_t i = 0; i < inputBatch.size(); ++i) {
        totalLoss += trainSample(inputBatch[i], targetBatch[i]);
    }
    
    return totalLoss / static_cast<T>(inputBatch.size());
}

template<typename T>
typename NeuralNetwork<T>::TrainingHistory 
NeuralNetwork<T>::train(const std::vector<std::vector<T>>& inputData,
                       const std::vector<std::vector<T>>& targetData,
                       std::size_t epochs,
                       std::size_t batchSize,
                       const std::vector<std::vector<T>>* validationInputs,
                       const std::vector<std::vector<T>>* validationTargets,
                       ProgressCallback progressCallback) {
    
    TrainingHistory history;
    isTraining_.store(true);
    shouldStop_.store(false);
    
    NNV_LOG_INFO("Starting training for network '{}': {} epochs, batch size {}", 
                name_, epochs, batchSize);
    
    // Create mutable copies for shuffling
    auto inputs = inputData;
    auto targets = targetData;
    
    for (std::size_t epoch = 0; epoch < epochs && !shouldStop_.load(); ++epoch) {
        // Shuffle data
        shuffleData(inputs, targets);
        
        // Create batches
        auto batches = createBatches(inputs, targets, batchSize);
        
        T epochLoss = T{0};
        for (const auto& batch : batches) {
            epochLoss += trainBatch(batch.first, batch.second);
        }
        epochLoss /= static_cast<T>(batches.size());
        
        // Compute training accuracy
        auto trainOutputs = predictBatch(inputs);
        T trainAccuracy = computeAccuracy(trainOutputs, targets);
        
        history.trainLoss.push_back(epochLoss);
        history.trainAccuracy.push_back(trainAccuracy);
        
        // Validation
        T valLoss = T{0};
        T valAccuracy = T{0};
        if (validationInputs && validationTargets) {
            auto valResult = evaluate(*validationInputs, *validationTargets);
            valLoss = valResult.first;
            valAccuracy = valResult.second;
            history.valLoss.push_back(valLoss);
            history.valAccuracy.push_back(valAccuracy);
        }
        
        // Update progress
        trainingProgress_.store(static_cast<T>(epoch + 1) / static_cast<T>(epochs));
        
        // Progress callback
        if (progressCallback) {
            progressCallback(epoch, epochLoss, trainAccuracy);
        }
        
        if (epoch % 10 == 0 || epoch == epochs - 1) {
            NNV_LOG_INFO("Epoch {}/{}: Loss = {:.6f}, Accuracy = {:.4f}", 
                        epoch + 1, epochs, epochLoss, trainAccuracy);
        }
    }
    
    isTraining_.store(false);
    trainingProgress_.store(T{1});
    
    NNV_LOG_INFO("Training completed for network '{}'", name_);
    return history;
}

template<typename T>
std::pair<T, T> NeuralNetwork<T>::evaluate(const std::vector<std::vector<T>>& inputData,
                                           const std::vector<std::vector<T>>& targetData) {
    if (inputData.size() != targetData.size()) {
        NNV_LOG_ERROR("Input data size {} doesn't match target data size {}", 
                     inputData.size(), targetData.size());
        return {T{0}, T{0}};
    }
    
    auto outputs = predictBatch(inputData);
    
    // Compute average loss
    T totalLoss = T{0};
    for (std::size_t i = 0; i < outputs.size(); ++i) {
        totalLoss += lossFunction_(outputs[i], targetData[i]);
    }
    T avgLoss = totalLoss / static_cast<T>(outputs.size());
    
    // Compute accuracy
    T accuracy = computeAccuracy(outputs, targetData);
    
    return {avgLoss, accuracy};
}

template<typename T>
std::vector<T> NeuralNetwork<T>::predict(const std::vector<T>& inputs) {
    bool wasTraining = isTraining_.load();
    isTraining_.store(false); // Disable dropout for inference
    
    auto outputs = forward(inputs);
    
    isTraining_.store(wasTraining); // Restore training state
    return outputs;
}

template<typename T>
std::vector<std::vector<T>> NeuralNetwork<T>::predictBatch(const std::vector<std::vector<T>>& inputBatch) {
    std::vector<std::vector<T>> outputs;
    outputs.reserve(inputBatch.size());
    
    for (const auto& inputs : inputBatch) {
        outputs.push_back(predict(inputs));
    }
    
    return outputs;
}

template<typename T>
void NeuralNetwork<T>::setLossType(LossType lossType) {
    lossType_ = lossType;
    updateLossFunction();
}

template<typename T>
void NeuralNetwork<T>::setOptimizerType(OptimizerType optimizerType) {
    optimizerType_ = optimizerType;
    updateOptimizer();
}

template<typename T>
void NeuralNetwork<T>::reset() {
    std::lock_guard<std::mutex> lock(networkMutex_);
    
    for (auto& layer : layers_) {
        layer->reset();
    }
    
    isTraining_.store(false);
    shouldStop_.store(false);
    trainingProgress_.store(T{0});
    
    NNV_LOG_DEBUG("Reset network '{}'", name_);
}

template<typename T>
nlohmann::json NeuralNetwork<T>::toJson() const {
    std::lock_guard<std::mutex> lock(networkMutex_);

    nlohmann::json json;
    json["name"] = name_;
    json["learning_rate"] = learningRate_;
    json["loss_type"] = static_cast<int>(lossType_);
    json["optimizer_type"] = static_cast<int>(optimizerType_);

    json["layers"] = nlohmann::json::array();
    for (const auto& layer : layers_) {
        json["layers"].push_back(layer->toJson());
    }

    return json;
}

template<typename T>
void NeuralNetwork<T>::fromJson(const nlohmann::json& json) {
    std::lock_guard<std::mutex> lock(networkMutex_);

    if (json.contains("name")) {
        name_ = json["name"].get<std::string>();
    }

    if (json.contains("learning_rate")) {
        learningRate_ = json["learning_rate"].get<T>();
    }

    if (json.contains("loss_type")) {
        lossType_ = static_cast<LossType>(json["loss_type"].get<int>());
        updateLossFunction();
    }

    if (json.contains("optimizer_type")) {
        optimizerType_ = static_cast<OptimizerType>(json["optimizer_type"].get<int>());
        updateOptimizer();
    }

    if (json.contains("layers") && json["layers"].is_array()) {
        layers_.clear();

        for (const auto& layerJson : json["layers"]) {
            auto layer = std::make_unique<Layer<T>>(1); // Temporary size
            layer->fromJson(layerJson);
            layers_.push_back(std::move(layer));
        }
    }
}

template<typename T>
bool NeuralNetwork<T>::saveToFile(const std::string& filename) const {
    try {
        auto json = toJson();
        std::ofstream file(filename);
        if (!file.is_open()) {
            NNV_LOG_ERROR("Failed to open file for writing: {}", filename);
            return false;
        }

        file << json.dump(4);
        NNV_LOG_INFO("Saved network '{}' to file: {}", name_, filename);
        return true;

    } catch (const std::exception& e) {
        NNV_LOG_ERROR("Failed to save network to {}: {}", filename, e.what());
        return false;
    }
}

template<typename T>
bool NeuralNetwork<T>::loadFromFile(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            NNV_LOG_ERROR("Failed to open file for reading: {}", filename);
            return false;
        }

        nlohmann::json json;
        file >> json;
        fromJson(json);

        NNV_LOG_INFO("Loaded network from file: {}", filename);
        return true;

    } catch (const std::exception& e) {
        NNV_LOG_ERROR("Failed to load network from {}: {}", filename, e.what());
        return false;
    }
}

template<typename T>
void NeuralNetwork<T>::updateLossFunction() {
    lossFunction_ = LossFactory::getFunction<T>(lossType_);
    lossGradientFunction_ = LossFactory::getGradient<T>(lossType_);
}

template<typename T>
void NeuralNetwork<T>::updateOptimizer() {
    // Optimizer implementation will be added later
    // For now, we use simple SGD which is already implemented in Layer::updateWeights
}

template<typename T>
T NeuralNetwork<T>::computeAccuracy(const std::vector<std::vector<T>>& outputs,
                                   const std::vector<std::vector<T>>& targets) const {
    if (outputs.empty() || targets.empty() || outputs.size() != targets.size()) {
        return T{0};
    }

    std::size_t correct = 0;

    for (std::size_t i = 0; i < outputs.size(); ++i) {
        if (outputs[i].size() == 1) {
            // Binary classification or regression
            T prediction = outputs[i][0] > T{0.5} ? T{1} : T{0};
            if (std::abs(prediction - targets[i][0]) < T{0.5}) {
                correct++;
            }
        } else {
            // Multi-class classification
            auto maxOutputIt = std::max_element(outputs[i].begin(), outputs[i].end());
            auto maxTargetIt = std::max_element(targets[i].begin(), targets[i].end());

            if (std::distance(outputs[i].begin(), maxOutputIt) ==
                std::distance(targets[i].begin(), maxTargetIt)) {
                correct++;
            }
        }
    }

    return static_cast<T>(correct) / static_cast<T>(outputs.size());
}

template<typename T>
void NeuralNetwork<T>::shuffleData(std::vector<std::vector<T>>& inputs,
                                  std::vector<std::vector<T>>& targets) const {
    if (inputs.size() != targets.size()) {
        return;
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    // Create indices and shuffle them
    std::vector<std::size_t> indices(inputs.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), gen);

    // Reorder data according to shuffled indices
    std::vector<std::vector<T>> shuffledInputs(inputs.size());
    std::vector<std::vector<T>> shuffledTargets(targets.size());

    for (std::size_t i = 0; i < indices.size(); ++i) {
        shuffledInputs[i] = std::move(inputs[indices[i]]);
        shuffledTargets[i] = std::move(targets[indices[i]]);
    }

    inputs = std::move(shuffledInputs);
    targets = std::move(shuffledTargets);
}

template<typename T>
std::vector<std::pair<std::vector<std::vector<T>>, std::vector<std::vector<T>>>>
NeuralNetwork<T>::createBatches(const std::vector<std::vector<T>>& inputs,
                               const std::vector<std::vector<T>>& targets,
                               std::size_t batchSize) const {
    std::vector<std::pair<std::vector<std::vector<T>>, std::vector<std::vector<T>>>> batches;

    for (std::size_t i = 0; i < inputs.size(); i += batchSize) {
        std::size_t endIdx = std::min(i + batchSize, inputs.size());

        std::vector<std::vector<T>> batchInputs(inputs.begin() + i, inputs.begin() + endIdx);
        std::vector<std::vector<T>> batchTargets(targets.begin() + i, targets.begin() + endIdx);

        batches.emplace_back(std::move(batchInputs), std::move(batchTargets));
    }

    return batches;
}

// Explicit template instantiations
template class NeuralNetwork<float>;
template class NeuralNetwork<double>;

} // namespace core
} // namespace nnv
