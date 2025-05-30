# Core API Reference

The Core API provides the fundamental neural network engine that powers the Neural Network Visualizer. This module contains all the essential classes and functions for creating, training, and managing neural networks.

## Table of Contents

- [NeuralNetwork Class](#neuralnetwork-class)
- [Layer Class](#layer-class)
- [Neuron Class](#neuron-class)
- [Activation Functions](#activation-functions)
- [Loss Functions](#loss-functions)
- [Weight Initializers](#weight-initializers)
- [Types and Enums](#types-and-enums)

---

## NeuralNetwork Class

The `NeuralNetwork<T>` class is the main interface for creating and managing neural networks.

### Template Parameters

- `T`: Numeric type for computations (typically `float` or `double`)

### Constructor

```cpp
NeuralNetwork(const NetworkConfig& config)
```

Creates a new neural network with the specified configuration.

**Parameters:**
- `config`: Configuration object specifying network architecture and training parameters

### Public Methods

#### Training Methods

```cpp
void train(const std::vector<T>& input, const std::vector<T>& target)
```

Trains the network on a single input-target pair using backpropagation.

**Parameters:**
- `input`: Input data vector
- `target`: Expected output vector

**Example:**
```cpp
std::vector<float> input = {0.5f, 0.3f};
std::vector<float> target = {1.0f};
network->train(input, target);
```

```cpp
void trainBatch(const std::vector<std::vector<T>>& inputs,
                const std::vector<std::vector<T>>& targets)
```

Trains the network on a batch of input-target pairs.

**Parameters:**
- `inputs`: Vector of input data vectors
- `targets`: Vector of expected output vectors

#### Prediction Methods

```cpp
std::vector<T> predict(const std::vector<T>& input)
```

Performs forward propagation and returns the network's output.

**Parameters:**
- `input`: Input data vector

**Returns:** Vector containing the network's output

**Example:**
```cpp
auto output = network->predict({0.5f, 0.3f});
std::cout << "Output: " << output[0] << std::endl;
```

```cpp
std::vector<std::vector<T>> predictBatch(const std::vector<std::vector<T>>& inputs)
```

Performs prediction on a batch of inputs.

#### Network Inspection

```cpp
size_t getLayerCount() const
```

Returns the number of layers in the network (including input and output layers).

```cpp
const Layer<T>& getLayer(size_t index) const
```

Returns a reference to the specified layer.

**Parameters:**
- `index`: Layer index (0-based)

```cpp
NetworkStats<T> getTrainingStats() const
```

Returns detailed statistics about the network's training progress.

#### Configuration Management

```cpp
void setLearningRate(T rate)
```

Updates the learning rate for training.

```cpp
T getLearningRate() const
```

Returns the current learning rate.

```cpp
void saveModel(const std::string& filepath) const
```

Saves the trained model to a file.

```cpp
void loadModel(const std::string& filepath)
```

Loads a previously saved model from a file.

---

## Layer Class

The `Layer<T>` class represents a single layer in the neural network.

### Constructor

```cpp
Layer(size_t inputSize, size_t outputSize,
      ActivationType activation = ActivationType::RELU,
      const WeightInitConfig& weightConfig = {})
```

**Parameters:**
- `inputSize`: Number of input connections
- `outputSize`: Number of neurons in this layer
- `activation`: Activation function type
- `weightConfig`: Weight initialization configuration

### Public Methods

```cpp
std::vector<T> forward(const std::vector<T>& input)
```

Performs forward propagation through the layer.

```cpp
std::vector<T> backward(const std::vector<T>& gradient)
```

Performs backward propagation through the layer.

```cpp
void updateWeights(T learningRate)
```

Updates layer weights based on computed gradients.

```cpp
size_t getNeuronCount() const
```

Returns the number of neurons in the layer.

```cpp
const Neuron<T>& getNeuron(size_t index) const
```

Returns a reference to the specified neuron.

---

## Neuron Class

The `Neuron<T>` class represents an individual neuron within a layer.

### Constructor

```cpp
Neuron(size_t inputCount,
       ActivationType activation = ActivationType::RELU)
```

### Public Methods

```cpp
T activate(const std::vector<T>& inputs)
```

Computes the neuron's activation given input values.

```cpp
T getOutput() const
```

Returns the neuron's last computed output.

```cpp
T getBias() const
```

Returns the neuron's bias value.

```cpp
void setBias(T bias)
```

Sets the neuron's bias value.

```cpp
const std::vector<T>& getWeights() const
```

Returns the neuron's weight vector.

```cpp
void setWeights(const std::vector<T>& weights)
```

Sets the neuron's weight vector.

---

## Activation Functions

The activation functions module provides various non-linear transformations.

### Available Functions

```cpp
namespace ActivationFunctions {
    T sigmoid(T x);
    T sigmoidDerivative(T x);

    T relu(T x);
    T reluDerivative(T x);

    T tanh(T x);
    T tanhDerivative(T x);

    T leakyRelu(T x, T alpha = 0.01);
    T leakyReluDerivative(T x, T alpha = 0.01);

    T elu(T x, T alpha = 1.0);
    T eluDerivative(T x, T alpha = 1.0);

    std::vector<T> softmax(const std::vector<T>& inputs);
    std::vector<T> softmaxDerivative(const std::vector<T>& outputs);
}
```

### Usage Example

```cpp
using namespace nnv::core::ActivationFunctions;

float input = 0.5f;
float activated = relu(input);
float derivative = reluDerivative(activated);
```

---

## Loss Functions

The loss functions module provides various cost functions for training.

### Available Functions

```cpp
namespace LossFunctions {
    T meanSquaredError(const std::vector<T>& predicted,
                      const std::vector<T>& actual);
    std::vector<T> meanSquaredErrorDerivative(const std::vector<T>& predicted,
                                             const std::vector<T>& actual);

    T crossEntropy(const std::vector<T>& predicted,
                   const std::vector<T>& actual);
    std::vector<T> crossEntropyDerivative(const std::vector<T>& predicted,
                                         const std::vector<T>& actual);

    T binaryCrossEntropy(const std::vector<T>& predicted,
                        const std::vector<T>& actual);
    std::vector<T> binaryCrossEntropyDerivative(const std::vector<T>& predicted,
                                               const std::vector<T>& actual);

    T huberLoss(const std::vector<T>& predicted,
                const std::vector<T>& actual, T delta = 1.0);
    std::vector<T> huberLossDerivative(const std::vector<T>& predicted,
                                      const std::vector<T>& actual, T delta = 1.0);
}
```

### Usage Example

```cpp
using namespace nnv::core::LossFunctions;

std::vector<float> predicted = {0.8f, 0.2f};
std::vector<float> actual = {1.0f, 0.0f};

float loss = crossEntropy(predicted, actual);
auto gradient = crossEntropyDerivative(predicted, actual);
```

---

## Weight Initializers

The weight initializers module provides various strategies for initializing network weights.

### Available Initializers

```cpp
namespace WeightInitializers {
    std::vector<T> random(size_t count, T min = -1.0, T max = 1.0);
    std::vector<T> xavier(size_t inputSize, size_t outputSize);
    std::vector<T> he(size_t inputSize, size_t outputSize);
    std::vector<T> lecun(size_t inputSize);
    std::vector<T> zeros(size_t count);
    std::vector<T> ones(size_t count);
    std::vector<T> normal(size_t count, T mean = 0.0, T stddev = 1.0);
}
```

### Usage Example

```cpp
using namespace nnv::core::WeightInitializers;

// Initialize weights for a layer with 10 inputs and 5 outputs
auto weights = xavier<float>(10, 5);

// Initialize with custom random range
auto biases = random<float>(5, -0.1f, 0.1f);
```

---

## Types and Enums

### NetworkConfig

Configuration structure for creating neural networks.

```cpp
struct NetworkConfig {
    size_t inputSize;                    // Number of input features
    std::vector<size_t> hiddenLayers;   // Size of each hidden layer
    size_t outputSize;                   // Number of output neurons
    ActivationType activationFunction;   // Default activation function
    LossType lossFunction;              // Loss function type
    T learningRate;                     // Training learning rate
    T momentum;                         // Momentum coefficient
    T weightDecay;                      // L2 regularization factor
    WeightInitType weightInitializer;   // Weight initialization method
    bool useBatchNorm;                  // Enable batch normalization
    T dropoutRate;                      // Dropout probability
};
```

### ActivationType

Enumeration of available activation functions.

```cpp
enum class ActivationType {
    SIGMOID,
    RELU,
    TANH,
    LEAKY_RELU,
    ELU,
    SWISH,
    SOFTMAX,
    LINEAR
};
```

### LossType

Enumeration of available loss functions.

```cpp
enum class LossType {
    MEAN_SQUARED_ERROR,
    CROSS_ENTROPY,
    BINARY_CROSS_ENTROPY,
    HUBER_LOSS,
    HINGE_LOSS
};
```

### WeightInitType

Enumeration of weight initialization strategies.

```cpp
enum class WeightInitType {
    RANDOM,
    XAVIER,
    HE,
    LECUN,
    ZEROS,
    ONES,
    NORMAL
};
```

### NetworkStats

Structure containing training statistics.

```cpp
struct NetworkStats {
    T currentLoss;           // Current loss value
    T averageLoss;          // Running average loss
    size_t epoch;           // Current training epoch
    size_t iterations;      // Total training iterations
    T accuracy;             // Classification accuracy (if applicable)
    std::vector<T> layerOutputs; // Output values for each layer
    std::chrono::milliseconds trainingTime; // Total training time
};
```

---

## Error Handling

The Core API uses exceptions for error handling:

### Exception Types

```cpp
class NeuralNetworkException : public std::runtime_error {
public:
    explicit NeuralNetworkException(const std::string& message);
};

class InvalidConfigException : public NeuralNetworkException {
public:
    explicit InvalidConfigException(const std::string& message);
};

class LayerMismatchException : public NeuralNetworkException {
public:
    explicit LayerMismatchException(const std::string& message);
};
```

### Example Error Handling

```cpp
try {
    NetworkConfig config;
    config.inputSize = 0;  // Invalid configuration
    auto network = std::make_unique<NeuralNetwork<float>>(config);
} catch (const InvalidConfigException& e) {
    std::cerr << "Configuration error: " << e.what() << std::endl;
} catch (const NeuralNetworkException& e) {
    std::cerr << "Neural network error: " << e.what() << std::endl;
}
```

---

## Thread Safety

The Core API is **not thread-safe** by default. If you need to use neural networks across multiple threads, you should:

1. Use separate network instances per thread
2. Implement your own synchronization mechanisms
3. Consider using the thread-safe utilities provided in the `utils` module

---

## Performance Considerations

### Memory Management

- Use `std::unique_ptr` or `std::shared_ptr` for network instances
- Consider using move semantics for large data transfers
- Enable compiler optimizations (`-O3` or `/O2`)

### SIMD Optimizations

The library automatically uses SIMD instructions when available:

```cpp
// Compile with SIMD support
#ifdef USE_AVX2
    // AVX2 optimized implementations will be used
#endif
```

### Batch Processing

For better performance, use batch methods when training on multiple samples:

```cpp
// Preferred: batch training
network->trainBatch(inputs, targets);

// Less efficient: individual training
for (size_t i = 0; i < inputs.size(); ++i) {
    network->train(inputs[i], targets[i]);
}
```

---

## Integration Examples

### Basic XOR Network

```cpp
#include "core/NeuralNetwork.hpp"

using namespace nnv::core;

int main() {
    NetworkConfig config;
    config.inputSize = 2;
    config.hiddenLayers = {4};
    config.outputSize = 1;
    config.activationFunction = ActivationType::SIGMOID;
    config.learningRate = 0.1f;

    auto network = std::make_unique<NeuralNetwork<float>>(config);

    // XOR training data
    std::vector<std::vector<float>> inputs = {
        {0, 0}, {0, 1}, {1, 0}, {1, 1}
    };
    std::vector<std::vector<float>> targets = {
        {0}, {1}, {1}, {0}
    };

    // Training loop
    for (int epoch = 0; epoch < 10000; ++epoch) {
        network->trainBatch(inputs, targets);

        if (epoch % 1000 == 0) {
            auto stats = network->getTrainingStats();
            std::cout << "Epoch " << epoch << ", Loss: " << stats.currentLoss << std::endl;
        }
    }

    // Test the network
    for (const auto& input : inputs) {
        auto output = network->predict(input);
        std::cout << "Input: [" << input[0] << ", " << input[1]
                  << "] -> Output: " << output[0] << std::endl;
    }

    return 0;
}
```

### Custom Activation Function

```cpp
#include "core/ActivationFunctions.hpp"

// Register custom activation function
float customActivation(float x) {
    return x / (1.0f + std::abs(x));  // Swish-like activation
}

float customActivationDerivative(float x) {
    float abs_x = std::abs(x);
    return 1.0f / ((1.0f + abs_x) * (1.0f + abs_x));
}

// Use in network configuration
NetworkConfig config;
config.activationFunction = ActivationType::CUSTOM;
// Set custom function pointers...
```

---

This API reference covers the core functionality of the Neural Network Visualizer's neural network engine. For more advanced usage patterns and integration examples, see the [tutorials](../tutorials/) section.