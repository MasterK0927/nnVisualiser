/**
 * @file Layer.cpp
 * @brief Implementation of the Layer class
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include "core/Layer.hpp"
#include "utils/Logger.hpp"
#include <cmath>
#include <random>
#include <algorithm>

namespace nnv { 
namespace core {

template<typename T>
Layer<T>::Layer(LayerSize size, ActivationType activation, const std::string& name)
    : neurons_(size)
    , name_(name)
    , activationType_(activation)
    , dropoutRate_(T{0})
    , trainable_(true)
    , dropoutMask_(size, true)
{
    initializeNeuronIds();
    updateActivationFunctions();
}

template<typename T>
Layer<T>::Layer(const LayerConfig& config)
    : neurons_(config.size)
    , name_(config.name)
    , activationType_(config.activation)
    , dropoutRate_(config.dropout_rate)
    , trainable_(config.trainable)
    , dropoutMask_(config.size, true)
{
    initializeNeuronIds();
    updateActivationFunctions();
}

template<typename T>
void Layer<T>::setActivationType(ActivationType type) {
    activationType_ = type;
    updateActivationFunctions();
}

template<typename T>
std::vector<T> Layer<T>::getActivations() const {
    std::vector<T> activations;
    activations.reserve(neurons_.size());
    
    for (const auto& neuron : neurons_) {
        activations.push_back(neuron.getActivation());
    }
    
    return activations;
}

template<typename T>
void Layer<T>::setActivations(const std::vector<T>& activations) {
    NNV_ASSERT(activations.size() == neurons_.size());
    
    for (std::size_t i = 0; i < neurons_.size(); ++i) {
        neurons_[i].setActivation(activations[i]);
    }
}

template<typename T>
std::vector<T> Layer<T>::getBiases() const {
    std::vector<T> biases;
    biases.reserve(neurons_.size());
    
    for (const auto& neuron : neurons_) {
        biases.push_back(neuron.getBias());
    }
    
    return biases;
}

template<typename T>
void Layer<T>::setBiases(const std::vector<T>& biases) {
    NNV_ASSERT(biases.size() == neurons_.size());
    
    for (std::size_t i = 0; i < neurons_.size(); ++i) {
        neurons_[i].setBias(biases[i]);
    }
}

template<typename T>
void Layer<T>::initializeWeights(LayerSize prevLayerSize, InitializationType initType) {
    switch (initType) {
        case InitializationType::Xavier:
            initializeXavier(prevLayerSize);
            break;
        case InitializationType::He:
            initializeHe(prevLayerSize);
            break;
        case InitializationType::Random:
            initializeRandom(prevLayerSize);
            break;
        case InitializationType::Zero:
            for (auto& neuron : neurons_) {
                neuron.setInputWeights(std::vector<T>(prevLayerSize, T{0}));
                neuron.setBias(T{0});
            }
            break;
        case InitializationType::One:
            for (auto& neuron : neurons_) {
                neuron.setInputWeights(std::vector<T>(prevLayerSize, T{1}));
                neuron.setBias(T{1});
            }
            break;
    }
}

template<typename T>
void Layer<T>::forward(const std::vector<T>& inputs) {
    NNV_ASSERT(!neurons_.empty());
    
    for (auto& neuron : neurons_) {
        const auto& weights = neuron.getInputWeights();
        NNV_ASSERT(weights.size() == inputs.size());
        
        // Compute weighted sum
        T weightedSum = T{0};
        for (std::size_t i = 0; i < inputs.size(); ++i) {
            weightedSum += inputs[i] * weights[i];
        }
        
        neuron.setWeightedInput(weightedSum);
    }
}

template<typename T>
void Layer<T>::applyActivation() {
    if (activationType_ == ActivationType::Softmax) {
        // Special handling for softmax
        std::vector<T> weightedInputs;
        weightedInputs.reserve(neurons_.size());
        
        for (const auto& neuron : neurons_) {
            weightedInputs.push_back(neuron.getWeightedInput() + neuron.getBias());
        }
        
        auto softmaxOutputs = activation::softmax(weightedInputs);
        
        for (std::size_t i = 0; i < neurons_.size(); ++i) {
            neurons_[i].setActivation(softmaxOutputs[i]);
        }
    } else {
        // Apply activation function to each neuron
        for (auto& neuron : neurons_) {
            neuron.applyActivation(activationFunc_);
        }
    }
}

template<typename T>
void Layer<T>::applyDropout(bool training) {
    if (!training || dropoutRate_ <= T{0}) {
        // No dropout during inference or if rate is 0
        std::fill(dropoutMask_.begin(), dropoutMask_.end(), true);
        return;
    }
    
    // Generate dropout mask
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<T> dist(T{0}, T{1});
    
    T keepProb = T{1} - dropoutRate_;
    
    for (std::size_t i = 0; i < dropoutMask_.size(); ++i) {
        dropoutMask_[i] = dist(gen) < keepProb;
        
        if (!dropoutMask_[i]) {
            neurons_[i].setActivation(T{0});
        } else {
            // Scale by keep probability to maintain expected value
            neurons_[i].setActivation(neurons_[i].getActivation() / keepProb);
        }
    }
}

template<typename T>
void Layer<T>::computeGradients(const std::vector<T>& nextLayerDeltas,
                                const std::vector<std::vector<T>>& nextLayerWeights) {
    NNV_ASSERT(nextLayerDeltas.size() == nextLayerWeights.size());
    
    for (std::size_t i = 0; i < neurons_.size(); ++i) {
        T delta = T{0};
        
        // Sum weighted deltas from next layer
        for (std::size_t j = 0; j < nextLayerDeltas.size(); ++j) {
            NNV_ASSERT(i < nextLayerWeights[j].size());
            delta += nextLayerDeltas[j] * nextLayerWeights[j][i];
        }
        
        // Multiply by activation derivative
        T activationDeriv = neurons_[i].computeActivationDerivative(activationDerivFunc_);
        delta *= activationDeriv;
        
        neurons_[i].setDelta(delta);
    }
}

template<typename T>
void Layer<T>::updateWeights(T learningRate, const std::vector<T>& prevLayerActivations) {
    if (!trainable_) {
        return;
    }
    
    for (auto& neuron : neurons_) {
        auto weights = neuron.getInputWeights();
        NNV_ASSERT(weights.size() == prevLayerActivations.size());
        
        T delta = neuron.getDelta();
        
        // Update weights
        for (std::size_t i = 0; i < weights.size(); ++i) {
            weights[i] -= learningRate * delta * prevLayerActivations[i];
        }
        
        // Update bias
        T bias = neuron.getBias();
        bias -= learningRate * delta;
        
        neuron.setInputWeights(weights);
        neuron.setBias(bias);
    }
}

template<typename T>
void Layer<T>::reset() {
    for (auto& neuron : neurons_) {
        neuron.reset();
    }
    std::fill(dropoutMask_.begin(), dropoutMask_.end(), true);
}

template<typename T>
std::vector<std::vector<T>> Layer<T>::getWeightMatrix() const {
    std::vector<std::vector<T>> weights;
    weights.reserve(neurons_.size());
    
    for (const auto& neuron : neurons_) {
        weights.push_back(neuron.getInputWeights());
    }
    
    return weights;
}

template<typename T>
void Layer<T>::setWeightMatrix(const std::vector<std::vector<T>>& weights) {
    NNV_ASSERT(weights.size() == neurons_.size());
    
    for (std::size_t i = 0; i < neurons_.size(); ++i) {
        neurons_[i].setInputWeights(weights[i]);
    }
}

template<typename T>
nlohmann::json Layer<T>::toJson() const {
    nlohmann::json json;
    
    json["name"] = name_;
    json["size"] = neurons_.size();
    json["activation_type"] = static_cast<int>(activationType_);
    json["dropout_rate"] = dropoutRate_;
    json["trainable"] = trainable_;
    
    json["neurons"] = nlohmann::json::array();
    for (const auto& neuron : neurons_) {
        json["neurons"].push_back(neuron.toJson());
    }
    
    return json;
}

template<typename T>
void Layer<T>::fromJson(const nlohmann::json& json) {
    if (json.contains("name")) {
        name_ = json["name"].get<std::string>();
    }
    
    if (json.contains("activation_type")) {
        activationType_ = static_cast<ActivationType>(json["activation_type"].get<int>());
        updateActivationFunctions();
    }
    
    if (json.contains("dropout_rate")) {
        dropoutRate_ = json["dropout_rate"].get<T>();
    }
    
    if (json.contains("trainable")) {
        trainable_ = json["trainable"].get<bool>();
    }
    
    if (json.contains("neurons") && json["neurons"].is_array()) {
        neurons_.clear();
        neurons_.reserve(json["neurons"].size());
        
        for (const auto& neuronJson : json["neurons"]) {
            Neuron<T> neuron;
            neuron.fromJson(neuronJson);
            neurons_.push_back(std::move(neuron));
        }
        
        dropoutMask_.resize(neurons_.size(), true);
    }
}

template<typename T>
void Layer<T>::updateActivationFunctions() {
    activationFunc_ = ActivationFactory::getFunction<T>(activationType_);
    activationDerivFunc_ = ActivationFactory::getDerivative<T>(activationType_);
}

template<typename T>
void Layer<T>::initializeNeuronIds() {
    for (std::size_t i = 0; i < neurons_.size(); ++i) {
        neurons_[i].setId(i);
    }
}

template<typename T>
void Layer<T>::initializeXavier(LayerSize prevLayerSize) {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    T limit = std::sqrt(T{6} / (prevLayerSize + neurons_.size()));
    std::uniform_real_distribution<T> dist(-limit, limit);
    
    for (auto& neuron : neurons_) {
        std::vector<T> weights(prevLayerSize);
        for (auto& weight : weights) {
            weight = dist(gen);
        }
        neuron.setInputWeights(weights);
        neuron.setBias(T{0}); // Initialize bias to zero
    }
}

template<typename T>
void Layer<T>::initializeHe(LayerSize prevLayerSize) {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    T stddev = std::sqrt(T{2} / prevLayerSize);
    std::normal_distribution<T> dist(T{0}, stddev);
    
    for (auto& neuron : neurons_) {
        std::vector<T> weights(prevLayerSize);
        for (auto& weight : weights) {
            weight = dist(gen);
        }
        neuron.setInputWeights(weights);
        neuron.setBias(T{0}); // Initialize bias to zero
    }
}

template<typename T>
void Layer<T>::initializeRandom(LayerSize prevLayerSize) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<T> dist(T{-1}, T{1});
    
    for (auto& neuron : neurons_) {
        std::vector<T> weights(prevLayerSize);
        for (auto& weight : weights) {
            weight = dist(gen);
        }
        neuron.setInputWeights(weights);
        neuron.setBias(dist(gen));
    }
}

// Explicit template instantiations
template class Layer<float>;
template class Layer<double>;

} // namespace core
} // namespace nnv
