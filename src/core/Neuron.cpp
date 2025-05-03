/**
 * @file Neuron.cpp
 * @brief Implementation of the Neuron class
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include "core/Neuron.hpp"
#include <nlohmann/json.hpp>

namespace nnv {
namespace core {

template<typename T>
Neuron<T>::Neuron(NeuronIndex id)
    : id_(id)
    , activation_(T{0})
    , bias_(T{0})
    , weightedInput_(T{0})
    , gradient_(T{0})
    , delta_(T{0})
    , trainable_(true)
    , name_("")
{
}

template<typename T>
void Neuron<T>::reset() {
    activation_ = T{0};
    weightedInput_ = T{0};
    gradient_ = T{0};
    delta_ = T{0};
}

template<typename T>
void Neuron<T>::applyActivation(const std::function<T(T)>& activationFunc) {
    activation_ = activationFunc(weightedInput_ + bias_);
}

template<typename T>
T Neuron<T>::computeActivationDerivative(const std::function<T(T)>& derivativeFunc) const {
    return derivativeFunc(weightedInput_ + bias_);
}

template<typename T>
nlohmann::json Neuron<T>::toJson() const {
    nlohmann::json json;
    
    json["id"] = id_;
    json["activation"] = activation_;
    json["bias"] = bias_;
    json["weighted_input"] = weightedInput_;
    json["gradient"] = gradient_;
    json["delta"] = delta_;
    json["trainable"] = trainable_;
    json["name"] = name_;
    json["input_weights"] = inputWeights_;
    
    return json;
}

template<typename T>
void Neuron<T>::fromJson(const nlohmann::json& json) {
    if (json.contains("id")) {
        id_ = json["id"].get<NeuronIndex>();
    }
    
    if (json.contains("activation")) {
        activation_ = json["activation"].get<T>();
    }
    
    if (json.contains("bias")) {
        bias_ = json["bias"].get<T>();
    }
    
    if (json.contains("weighted_input")) {
        weightedInput_ = json["weighted_input"].get<T>();
    }
    
    if (json.contains("gradient")) {
        gradient_ = json["gradient"].get<T>();
    }
    
    if (json.contains("delta")) {
        delta_ = json["delta"].get<T>();
    }
    
    if (json.contains("trainable")) {
        trainable_ = json["trainable"].get<bool>();
    }
    
    if (json.contains("name")) {
        name_ = json["name"].get<std::string>();
    }
    
    if (json.contains("input_weights")) {
        inputWeights_ = json["input_weights"].get<std::vector<T>>();
    }
}

// Explicit template instantiations
template class Neuron<float>;
template class Neuron<double>;

} // namespace core
} // namespace nnv
