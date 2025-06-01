# Building Your First Neural Network

This tutorial will guide you through creating, training, and visualizing your first neural network using the Neural Network Visualizer. We'll solve the classic XOR problem step by step.

## Table of Contents

- [What You'll Learn](#what-youll-learn)
- [Prerequisites](#prerequisites)
- [Understanding the XOR Problem](#understanding-the-xor-problem)
- [Setting Up Your First Network](#setting-up-your-first-network)
- [Training the Network](#training-the-network)
- [Visualizing the Results](#visualizing-the-results)
- [Understanding the Code](#understanding-the-code)
- [Experimenting Further](#experimenting-further)
- [Common Issues and Solutions](#common-issues-and-solutions)

---

## What You'll Learn

By the end of this tutorial, you will:

- Understand how to create a basic neural network configuration
- Learn to train a network using the XOR dataset
- Visualize training progress and network behavior
- Interpret neural network outputs
- Experiment with different network parameters

## Prerequisites

- Neural Network Visualizer installed and working
- Basic understanding of neural networks (helpful but not required)
- Familiarity with C++ basics

## Understanding the XOR Problem

The XOR (exclusive OR) problem is a classic example in neural network literature because it's not linearly separable. Here's what XOR means:

| Input A | Input B | XOR Output |
|---------|---------|------------|
|    0    |    0    |     0      |
|    0    |    1    |     1      |
|    1    |    0    |     1      |
|    1    |    1    |     0      |

The output is 1 only when exactly one of the inputs is 1. This requires at least one hidden layer to solve.

---

## Setting Up Your First Network

### Step 1: Create a New Project File

Create a file named `my_first_network.cpp`:

```cpp
#include <iostream>
#include <vector>
#include <memory>
#include <iomanip>

// Neural Network Visualizer includes
#include "core/NeuralNetwork.hpp"
#include "core/Types.hpp"
#include "utils/Logger.hpp"

using namespace nnv::core;
using namespace nnv::utils;

int main() {
    // Set up logging to see what's happening
    Logger::setLevel(LogLevel::INFO);
    Logger::info("Starting first neural network tutorial");

    return 0;
}
```

### Step 2: Configure the Network

Add the network configuration inside your `main()` function:

```cpp
int main() {
    Logger::setLevel(LogLevel::INFO);
    Logger::info("Starting first neural network tutorial");

    // Create network configuration
    NetworkConfig config;

    // Input layer: 2 neurons (for A and B inputs)
    config.inputSize = 2;

    // Hidden layer: 4 neurons (experiment with this!)
    config.hiddenLayers = {4};

    // Output layer: 1 neuron (for XOR result)
    config.outputSize = 1;

    // Choose activation function
    config.activationFunction = ActivationType::SIGMOID;

    // Set learning parameters
    config.learningRate = 0.5f;        // How fast the network learns
    config.momentum = 0.0f;            // We'll start without momentum
    config.weightDecay = 0.0f;         // No regularization for now

    // Weight initialization
    config.weightInitializer = WeightInitType::XAVIER;

    Logger::info("Network configuration created");

    return 0;
}
```

### Step 3: Create the Network

Add the network creation:

```cpp
    // Create the neural network
    try {
        auto network = std::make_unique<NeuralNetwork<float>>(config);
        Logger::info("Neural network created successfully");

        // Display network information
        std::cout << "Network Architecture:" << std::endl;
        std::cout << "- Input neurons: " << config.inputSize << std::endl;
        std::cout << "- Hidden layers: ";
        for (size_t layerSize : config.hiddenLayers) {
            std::cout << layerSize << " ";
        }
        std::cout << std::endl;
        std::cout << "- Output neurons: " << config.outputSize << std::endl;
        std::cout << "- Total layers: " << network->getLayerCount() << std::endl;

    } catch (const std::exception& e) {
        Logger::error("Failed to create network: " + std::string(e.what()));
        return 1;
    }
```

---

## Training the Network

### Step 4: Prepare Training Data

Add the XOR training data:

```cpp
    // XOR training data
    std::vector<std::vector<float>> trainingInputs = {
        {0.0f, 0.0f},  // Input: A=0, B=0
        {0.0f, 1.0f},  // Input: A=0, B=1
        {1.0f, 0.0f},  // Input: A=1, B=0
        {1.0f, 1.0f}   // Input: A=1, B=1
    };

    std::vector<std::vector<float>> trainingTargets = {
        {0.0f},        // Expected: 0
        {1.0f},        // Expected: 1
        {1.0f},        // Expected: 1
        {0.0f}         // Expected: 0
    };

    Logger::info("Training data prepared");

    // Verify data
    std::cout << "\nTraining Data:" << std::endl;
    for (size_t i = 0; i < trainingInputs.size(); ++i) {
        std::cout << "Input: [" << trainingInputs[i][0] << ", "
                  << trainingInputs[i][1] << "] -> Target: "
                  << trainingTargets[i][0] << std::endl;
    }
```

### Step 5: Training Loop

Add the main training loop:

```cpp
    // Training parameters
    const int maxEpochs = 10000;
    const float targetError = 0.01f;
    const int printInterval = 1000;

    std::cout << "\nStarting training..." << std::endl;
    std::cout << "Target error: " << targetError << std::endl;
    std::cout << "Maximum epochs: " << maxEpochs << std::endl;

    // Training loop
    for (int epoch = 0; epoch < maxEpochs; ++epoch) {
        float totalError = 0.0f;

        // Train on each sample
        for (size_t i = 0; i < trainingInputs.size(); ++i) {
            // Train the network
            network->train(trainingInputs[i], trainingTargets[i]);

            // Calculate error for monitoring
            auto output = network->predict(trainingInputs[i]);
            float error = std::abs(output[0] - trainingTargets[i][0]);
            totalError += error;
        }

        // Average error across all samples
        float averageError = totalError / trainingInputs.size();

        // Print progress
        if (epoch % printInterval == 0) {
            std::cout << "Epoch " << epoch << ", Average Error: "
                      << std::fixed << std::setprecision(6) << averageError << std::endl;
        }

        // Check if we've reached our target
        if (averageError < targetError) {
            std::cout << "Training completed! Final error: " << averageError
                      << " at epoch " << epoch << std::endl;
            break;
        }
    }
```

### Step 6: Test the Trained Network

Add testing code after the training loop:

```cpp
    std::cout << "\nTesting the trained network:" << std::endl;
    std::cout << "Input\t\tTarget\t\tOutput\t\tError" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;

    for (size_t i = 0; i < trainingInputs.size(); ++i) {
        auto output = network->predict(trainingInputs[i]);
        float error = std::abs(output[0] - trainingTargets[i][0]);

        std::cout << "[" << trainingInputs[i][0] << ", " << trainingInputs[i][1] << "]\t\t"
                  << trainingTargets[i][0] << "\t\t"
                  << std::fixed << std::setprecision(4) << output[0] << "\t\t"
                  << std::setprecision(6) << error << std::endl;
    }
```

---

## Visualizing the Results

### Step 7: Add Network Statistics

To better understand what happened during training:

```cpp
    // Display network statistics
    auto stats = network->getTrainingStats();

    std::cout << "\nNetwork Statistics:" << std::endl;
    std::cout << "- Final loss: " << stats.currentLoss << std::endl;
    std::cout << "- Total iterations: " << stats.iterations << std::endl;
    std::cout << "- Training time: " << stats.trainingTime.count() << " ms" << std::endl;

    // Let's also look at the network weights (optional)
    std::cout << "\nNetwork Weights (Layer 0 to 1):" << std::endl;
    const auto& firstLayer = network->getLayer(1); // First hidden layer
    for (size_t i = 0; i < firstLayer.getNeuronCount(); ++i) {
        const auto& neuron = firstLayer.getNeuron(i);
        const auto& weights = neuron.getWeights();

        std::cout << "Neuron " << i << " weights: [";
        for (size_t j = 0; j < weights.size(); ++j) {
            std::cout << std::fixed << std::setprecision(3) << weights[j];
            if (j < weights.size() - 1) std::cout << ", ";
        }
        std::cout << "], bias: " << neuron.getBias() << std::endl;
    }
```

---

## Understanding the Code

Let's break down what each part does:

### Network Configuration

```cpp
NetworkConfig config;
config.inputSize = 2;        // Two inputs: A and B
config.hiddenLayers = {4};   // One hidden layer with 4 neurons
config.outputSize = 1;       // One output: XOR result
```

This creates a network with the topology: 2 → 4 → 1

### Training Process

```cpp
network->train(trainingInputs[i], trainingTargets[i]);
```

This line:
1. Passes input through the network (forward propagation)
2. Compares output with target (error calculation)
3. Adjusts weights to reduce error (backpropagation)

### Activation Function

We used `SIGMOID` because:
- It outputs values between 0 and 1 (perfect for our binary problem)
- It's differentiable (needed for backpropagation)
- It's a good choice for beginners

---

## Experimenting Further

Now that you have a working network, try these experiments:

### Experiment 1: Different Hidden Layer Sizes

Try these configurations and see how they affect training:

```cpp
// Small network - might struggle
config.hiddenLayers = {2};

// Larger network - might overfit
config.hiddenLayers = {8};

// Multiple hidden layers
config.hiddenLayers = {4, 3};

// Very deep network
config.hiddenLayers = {6, 4, 3};
```

### Experiment 2: Different Activation Functions

```cpp
// Try ReLU (often faster training)
config.activationFunction = ActivationType::RELU;

// Try Tanh (symmetric around zero)
config.activationFunction = ActivationType::TANH;
```

### Experiment 3: Learning Rate Effects

```cpp
// Very slow learning
config.learningRate = 0.01f;

// Very fast learning (might be unstable)
config.learningRate = 2.0f;

// Adaptive learning rate (advanced)
// You could implement decreasing learning rate over time
```

### Experiment 4: Add Momentum

```cpp
// Add momentum to help with convergence
config.momentum = 0.9f;
```

### Experiment 5: Noise Robustness

Add some noise to test robustness:

```cpp
// Test with slightly noisy inputs
std::vector<std::vector<float>> noisyInputs = {
    {0.05f, 0.02f},  // Should still be ~[0,0]
    {0.01f, 0.98f},  // Should still be ~[0,1]
    {0.99f, 0.03f},  // Should still be ~[1,0]
    {0.97f, 0.95f}   // Should still be ~[1,1]
};
```

---

## Complete Working Example

Here's the complete `my_first_network.cpp` file:

```cpp
#include <iostream>
#include <vector>
#include <memory>
#include <iomanip>

#include "core/NeuralNetwork.hpp"
#include "core/Types.hpp"
#include "utils/Logger.hpp"

using namespace nnv::core;
using namespace nnv::utils;

int main() {
    Logger::setLevel(LogLevel::INFO);
    Logger::info("Starting first neural network tutorial");

    // Network configuration
    NetworkConfig config;
    config.inputSize = 2;
    config.hiddenLayers = {4};
    config.outputSize = 1;
    config.activationFunction = ActivationType::SIGMOID;
    config.learningRate = 0.5f;
    config.momentum = 0.0f;
    config.weightDecay = 0.0f;
    config.weightInitializer = WeightInitType::XAVIER;

    // Create network
    auto network = std::make_unique<NeuralNetwork<float>>(config);

    // Training data
    std::vector<std::vector<float>> inputs = {
        {0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}
    };
    std::vector<std::vector<float>> targets = {
        {0.0f}, {1.0f}, {1.0f}, {0.0f}
    };

    // Training
    const int maxEpochs = 10000;
    const float targetError = 0.01f;

    for (int epoch = 0; epoch < maxEpochs; ++epoch) {
        float totalError = 0.0f;

        for (size_t i = 0; i < inputs.size(); ++i) {
            network->train(inputs[i], targets[i]);
            auto output = network->predict(inputs[i]);
            totalError += std::abs(output[0] - targets[i][0]);
        }

        float avgError = totalError / inputs.size();

        if (epoch % 1000 == 0) {
            std::cout << "Epoch " << epoch << ", Error: " << avgError << std::endl;
        }

        if (avgError < targetError) {
            std::cout << "Training completed at epoch " << epoch << std::endl;
            break;
        }
    }

    // Testing
    std::cout << "\nResults:" << std::endl;
    for (size_t i = 0; i < inputs.size(); ++i) {
        auto output = network->predict(inputs[i]);
        std::cout << "Input: [" << inputs[i][0] << ", " << inputs[i][1]
                  << "] -> Output: " << std::fixed << std::setprecision(4)
                  << output[0] << " (Target: " << targets[i][0] << ")" << std::endl;
    }

    return 0;
}
```

### Building and Running

```bash
# Add to your CMakeLists.txt or compile directly
g++ -std=c++17 -I../include my_first_network.cpp -o my_first_network -L../build/vcpkg/src -lnnv_core -lnnv_utils

# Run your program
./my_first_network
```

---

## Common Issues and Solutions

### Issue 1: Network Won't Learn

**Symptoms**: Error stays high, doesn't decrease

**Solutions**:
- Try a larger hidden layer (e.g., {8} instead of {4})
- Increase learning rate (e.g., 1.0 instead of 0.5)
- Try different weight initialization
- Increase maximum epochs

### Issue 2: Training is Unstable

**Symptoms**: Error jumps around, doesn't converge smoothly

**Solutions**:
- Decrease learning rate (e.g., 0.1 instead of 0.5)
- Add momentum (e.g., 0.9)
- Try different activation function

### Issue 3: Overfitting

**Symptoms**: Perfect on training data, poor on new data

**Solutions**:
- Use smaller hidden layers
- Add weight decay (regularization)
- Use dropout (advanced topic)

### Issue 4: Compilation Errors

**Common fixes**:
- Ensure all include paths are correct
- Link with required libraries (`-lnnv_core -lnnv_utils`)
- Use C++17 compiler flags

---

## Next Steps

Congratulations! You've built your first neural network. Here's what to explore next:

1. **[Understanding Backpropagation](backpropagation.md)** - Learn how neural networks actually learn
2. **[Advanced Network Architectures](advanced-architectures.md)** - Build more complex networks
3. **[Working with Real Data](../examples/mnist-tutorial.md)** - Move beyond toy problems
4. **[Visualization Features](../guides/ui-guide.md)** - Use the GUI to visualize your networks

### Challenges to Try

1. **AND/OR Gates**: Modify the code to learn AND or OR logic gates
2. **Multi-Class Classification**: Extend to classify into 3+ categories
3. **Regression Problem**: Try predicting continuous values instead of binary
4. **Time Series**: Predict the next value in a sequence

---

## Summary

In this tutorial, you learned:

- ✅ How to configure a neural network
- ✅ How to prepare training data
- ✅ How to train a network using backpropagation
- ✅ How to test and evaluate results
- ✅ How to experiment with different parameters
- ✅ Common issues and how to solve them

You now have the foundation to build more complex neural networks and tackle real-world problems!