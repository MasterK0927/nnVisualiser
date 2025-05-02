/**
 * @file Types.hpp
 * @brief Common type definitions for the Neural Network Visualizer
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>
#include <cstdint>

#ifdef HAS_EIGEN3
#include <Eigen/Dense>
#endif

namespace nnv {
namespace core {

// Precision types
using Float = float;
using Double = double;

// Default precision
using Scalar = Float;

// Matrix and vector types
#ifdef HAS_EIGEN3
using Matrix = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;
using Vector = Eigen::Vector<Scalar, Eigen::Dynamic>;
using RowVector = Eigen::RowVector<Scalar, Eigen::Dynamic>;
#else
using Matrix = std::vector<std::vector<Scalar>>;
using Vector = std::vector<Scalar>;
using RowVector = std::vector<Scalar>;
#endif

// Network architecture types
using LayerSize = std::size_t;
using LayerIndex = std::size_t;
using NeuronIndex = std::size_t;

// Activation function types
enum class ActivationType {
    None,
    ReLU,
    Sigmoid,
    Tanh,
    LeakyReLU,
    ELU,
    Swish,
    GELU,
    Softmax
};

// Optimizer types
enum class OptimizerType {
    SGD,
    Adam,
    RMSprop,
    AdaGrad
};

// Loss function types
enum class LossType {
    MeanSquaredError,
    CrossEntropy,
    BinaryCrossEntropy,
    Huber,
    FocalLoss
};

// Weight initialization types
enum class InitializationType {
    Random,
    Xavier,
    He,
    Zero,
    One
};

// Training configuration
struct TrainingConfig {
    Scalar learning_rate = 0.001f;
    std::size_t batch_size = 32;
    std::size_t epochs = 100;
    Scalar validation_split = 0.2f;
    bool shuffle = true;
    std::size_t early_stopping_patience = 10;
    Scalar early_stopping_min_delta = 1e-4f;
};

// Layer configuration
struct LayerConfig {
    LayerSize size;
    ActivationType activation = ActivationType::ReLU;
    Scalar dropout_rate = 0.0f;
    InitializationType weight_init = InitializationType::Xavier;
    std::string name;
    bool trainable = true;
};

// Network configuration
struct NetworkConfig {
    std::vector<LayerConfig> layers;
    OptimizerType optimizer = OptimizerType::Adam;
    LossType loss = LossType::MeanSquaredError;
    TrainingConfig training;
    std::string name = "Neural Network";
};

// Forward declarations
template<typename T = Scalar>
class NeuralNetwork;

template<typename T = Scalar>
class Layer;

template<typename T = Scalar>
class Neuron;

// Type aliases for default precision
using DefaultNetwork = NeuralNetwork<Scalar>;
using DefaultLayer = Layer<Scalar>;
using DefaultNeuron = Neuron<Scalar>;

// Callback types
using ProgressCallback = std::function<void(std::size_t epoch, Scalar loss, Scalar accuracy)>;
using ValidationCallback = std::function<bool(std::size_t epoch, Scalar val_loss, Scalar val_accuracy)>;

// Error handling
enum class ErrorCode {
    Success = 0,
    InvalidInput,
    InvalidConfiguration,
    FileNotFound,
    ParseError,
    MemoryError,
    ComputationError,
    UnknownError
};

struct Result {
    ErrorCode code = ErrorCode::Success;
    std::string message;
    
    bool isSuccess() const { return code == ErrorCode::Success; }
    bool isError() const { return code != ErrorCode::Success; }
    
    static Result success() { return {ErrorCode::Success, ""}; }
    static Result error(ErrorCode code, const std::string& msg) { return {code, msg}; }
};

} // namespace core
} // namespace nnv
