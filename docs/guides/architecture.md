# Architecture Overview

This document provides a comprehensive overview of the Neural Network Visualizer's architecture, design patterns, and system organization. Understanding this architecture will help you navigate the codebase and contribute effectively.

## Table of Contents

- [High-Level Architecture](#high-level-architecture)
- [Module Overview](#module-overview)
- [Core Components](#core-components)
- [Data Flow](#data-flow)
- [Design Patterns](#design-patterns)
- [Memory Management](#memory-management)
- [Threading Model](#threading-model)
- [Plugin Architecture](#plugin-architecture)
- [Performance Considerations](#performance-considerations)
- [Future Extensibility](#future-extensibility)

---

## High-Level Architecture

The Neural Network Visualizer follows a layered architecture with clear separation of concerns:

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                        │
│  ┌─────────────────┐    ┌─────────────────┐               │
│  │   GUI Frontend  │    │  CLI Interface  │               │
│  │     (SFML)      │    │                 │               │
│  └─────────────────┘    └─────────────────┘               │
└─────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────────────┐
│                    Presentation Layer                       │
│  ┌─────────────────┐    ┌─────────────────┐               │
│  │   UI Manager    │    │  Event System   │               │
│  │    (ImGui)      │    │                 │               │
│  └─────────────────┘    └─────────────────┘               │
└─────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────────────┐
│                   Visualization Layer                       │
│  ┌─────────────────┐    ┌─────────────────┐               │
│  │    Graphics     │    │   Animation     │               │
│  │    Engine       │    │    System       │               │
│  └─────────────────┘    └─────────────────┘               │
└─────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────────────┐
│                     Business Layer                          │
│  ┌─────────────────┐    ┌─────────────────┐               │
│  │ Neural Network  │    │    Training     │               │
│  │    Engine       │    │    Manager      │               │
│  └─────────────────┘    └─────────────────┘               │
└─────────────────────────────────────────────────────────────┘
┌─────────────────────────────────────────────────────────────┐
│                   Infrastructure Layer                      │
│  ┌─────────────────┐ ┌─────────────────┐ ┌─────────────────┐│
│  │     Logging     │ │  Configuration  │ │   Data I/O      ││
│  │                 │ │    Management   │ │                 ││
│  └─────────────────┘ └─────────────────┘ └─────────────────┘│
└─────────────────────────────────────────────────────────────┘
```

### Key Principles

1. **Separation of Concerns**: Each layer has a specific responsibility
2. **Dependency Inversion**: Higher layers depend on abstractions, not implementations
3. **Single Responsibility**: Each module has one reason to change
4. **Open/Closed Principle**: Open for extension, closed for modification
5. **Interface Segregation**: Clients depend only on interfaces they use

---

## Module Overview

### Core Module (`src/core/`, `include/core/`)

The heart of the neural network engine, containing:

```cpp
namespace nnv::core {
    // Main neural network class
    template<typename T>
    class NeuralNetwork;

    // Individual layer representation
    template<typename T>
    class Layer;

    // Individual neuron representation
    template<typename T>
    class Neuron;

    // Activation functions
    namespace ActivationFunctions { /* ... */ }

    // Loss functions
    namespace LossFunctions { /* ... */ }

    // Weight initialization strategies
    namespace WeightInitializers { /* ... */ }

    // Type definitions and configuration
    // Types.hpp contains all common types and enums
}
```

**Key Files:**
- `NeuralNetwork.hpp/cpp`: Main network implementation
- `Layer.hpp/cpp`: Layer abstraction and operations
- `Neuron.hpp/cpp`: Individual neuron implementation
- `ActivationFunctions.hpp/cpp`: Activation function library
- `LossFunctions.hpp/cpp`: Cost function implementations
- `WeightInitializers.hpp/cpp`: Weight initialization strategies
- `Types.hpp`: Common types, enums, and configuration structures

### Graphics Module (`src/graphics/`, `include/graphics/`)

Handles all visualization and rendering:

```cpp
namespace nnv::graphics {
    // Main visualization coordinator
    class Visualizer;

    // Network rendering engine
    class NetworkRenderer;

    // Individual layer rendering
    class LayerRenderer;

    // Animation system
    class AnimationSystem;

    // Color scheme management
    class ColorScheme;

    // Rendering configuration
    class RenderConfig;
}
```

**Key Files:**
- `Visualizer.hpp/cpp`: Main visualization coordinator
- `NetworkRenderer.hpp/cpp`: Network-specific rendering
- `AnimationSystem.hpp/cpp`: Smooth animations and transitions
- `ColorScheme.hpp/cpp`: Color management and themes
- `RenderConfig.hpp/cpp`: Rendering settings and configuration

### UI Module (`src/ui/`, `include/ui/`)

User interface components and management:

```cpp
namespace nnv::ui {
    // Main UI coordinator
    class UIManager;

    // Individual UI panels
    class NetworkPanel;
    class TrainingPanel;
    class ConfigPanel;

    // Base panel class
    class UIPanel;

    // Input handling
    class InputHandler;
}
```

**Key Files:**
- `UIManager.hpp/cpp`: Main UI coordination
- `NetworkPanel.hpp/cpp`: Network configuration panel
- `UIPanel.hpp/cpp`: Base class for all panels
- `InputHandler.hpp/cpp`: Mouse and keyboard input

### Utils Module (`src/utils/`, `include/utils/`)

Utility functions and infrastructure:

```cpp
namespace nnv::utils {
    // Configuration management
    class ConfigManager;

    // Data loading and preprocessing
    template<typename T>
    class DataLoader;

    // Logging system
    class Logger;

    // Mathematical utilities
    namespace MathUtils { /* ... */ }

    // Common utility functions
    namespace Common { /* ... */ }
}
```

**Key Files:**
- `ConfigManager.hpp/cpp`: Configuration loading and saving
- `DataLoader.hpp/cpp`: Dataset loading and preprocessing
- `Logger.hpp/cpp`: Logging infrastructure
- `Common.hpp/cpp`: Common utility functions

---

## Core Components

### Neural Network Engine

The neural network engine is designed for flexibility and performance:

```cpp
template<typename T>
class NeuralNetwork {
private:
    std::vector<std::unique_ptr<Layer<T>>> layers_;
    std::unique_ptr<LossFunction<T>> lossFunction_;
    TrainingConfig trainingConfig_;
    NetworkStats<T> stats_;

public:
    // Construction
    explicit NeuralNetwork(const NetworkConfig& config);

    // Training interface
    void train(const std::vector<T>& input, const std::vector<T>& target);
    void trainBatch(const std::vector<std::vector<T>>& inputs,
                   const std::vector<std::vector<T>>& targets);

    // Inference interface
    std::vector<T> predict(const std::vector<T>& input);
    std::vector<std::vector<T>> predictBatch(const std::vector<std::vector<T>>& inputs);

    // Introspection
    const Layer<T>& getLayer(size_t index) const;
    const NetworkStats<T>& getStats() const;
};
```

#### Layer Implementation

```cpp
template<typename T>
class Layer {
private:
    std::vector<std::unique_ptr<Neuron<T>>> neurons_;
    std::unique_ptr<ActivationFunction<T>> activation_;
    LayerType type_;

public:
    // Forward propagation
    std::vector<T> forward(const std::vector<T>& inputs);

    // Backward propagation
    std::vector<T> backward(const std::vector<T>& gradients);

    // Weight updates
    void updateWeights(T learningRate);

    // Introspection
    size_t getNeuronCount() const;
    const Neuron<T>& getNeuron(size_t index) const;
};
```

### Visualization System

The visualization system separates rendering logic from neural network logic:

```cpp
class NetworkRenderer {
private:
    std::unique_ptr<sf::RenderWindow> window_;
    std::unique_ptr<AnimationSystem> animator_;
    RenderConfig config_;

public:
    // Main rendering loop
    void render(const NeuralNetwork<float>& network);

    // Specific rendering functions
    void renderLayer(const Layer<float>& layer, const LayerRenderInfo& info);
    void renderNeuron(const Neuron<float>& neuron, const NeuronRenderInfo& info);
    void renderConnections(const Layer<float>& from, const Layer<float>& to);

    // Animation control
    void animateActivation(size_t layerIndex, size_t neuronIndex, float value);
    void animateWeightUpdate(size_t fromLayer, size_t fromNeuron,
                           size_t toLayer, size_t toNeuron, float delta);
};
```

---

## Data Flow

### Training Data Flow

```
Input Data
    │
    ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│ Data Loader │───▶│Preprocessing│───▶│   Network   │
└─────────────┘    └─────────────┘    └─────────────┘
                                           │
                                           ▼
                                    ┌─────────────┐
                                    │   Forward   │
                                    │Propagation  │
                                    └─────────────┘
                                           │
                                           ▼
                                    ┌─────────────┐
                                    │ Loss        │
                                    │Calculation  │
                                    └─────────────┘
                                           │
                                           ▼
                                    ┌─────────────┐
                                    │ Backward    │
                                    │Propagation  │
                                    └─────────────┘
                                           │
                                           ▼
                                    ┌─────────────┐
                                    │   Weight    │
                                    │   Update    │
                                    └─────────────┘
```

### Visualization Data Flow

```
Neural Network State
         │
         ▼
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│  State      │───▶│ Animation   │───▶│  Rendering  │
│ Extraction  │    │   System    │    │   Engine    │
└─────────────┘    └─────────────┘    └─────────────┘
         │                                   │
         ▼                                   ▼
┌─────────────┐                      ┌─────────────┐
│   Events    │                      │   Display   │
│  (Changes)  │                      │             │
└─────────────┘                      └─────────────┘
```

### Event System

The application uses an event-driven architecture for UI interactions:

```cpp
class EventSystem {
public:
    // Event registration
    template<typename EventType>
    void subscribe(std::function<void(const EventType&)> handler);

    // Event dispatch
    template<typename EventType>
    void publish(const EventType& event);

private:
    std::unordered_map<std::type_index, std::vector<std::function<void(const void*)>>> handlers_;
};

// Common events
struct NetworkStateChanged {
    const NeuralNetwork<float>* network;
    ChangeType type;
};

struct TrainingEpochCompleted {
    size_t epoch;
    float loss;
    float accuracy;
};

struct UIInteraction {
    InteractionType type;
    sf::Vector2f position;
};
```

---

## Design Patterns

### 1. Template Method Pattern

Used in the neural network training process:

```cpp
template<typename T>
class TrainingStrategy {
public:
    void train(NeuralNetwork<T>& network, const TrainingData<T>& data) {
        preprocessData(data);
        for (int epoch = 0; epoch < getMaxEpochs(); ++epoch) {
            performEpoch(network, data);
            if (shouldStop(network)) break;
            updateParameters(epoch);
        }
        postProcess(network);
    }

protected:
    virtual void preprocessData(const TrainingData<T>& data) = 0;
    virtual void performEpoch(NeuralNetwork<T>& network, const TrainingData<T>& data) = 0;
    virtual bool shouldStop(const NeuralNetwork<T>& network) = 0;
    virtual void updateParameters(int epoch) = 0;
    virtual void postProcess(NeuralNetwork<T>& network) = 0;
    virtual int getMaxEpochs() const = 0;
};
```

### 2. Strategy Pattern

Used for different activation functions:

```cpp
template<typename T>
class ActivationFunction {
public:
    virtual ~ActivationFunction() = default;
    virtual T activate(T input) const = 0;
    virtual T derivative(T input) const = 0;
    virtual std::unique_ptr<ActivationFunction<T>> clone() const = 0;
};

template<typename T>
class ReLUActivation : public ActivationFunction<T> {
public:
    T activate(T input) const override {
        return std::max(T(0), input);
    }

    T derivative(T input) const override {
        return input > T(0) ? T(1) : T(0);
    }

    std::unique_ptr<ActivationFunction<T>> clone() const override {
        return std::make_unique<ReLUActivation<T>>();
    }
};
```

### 3. Observer Pattern

Used for training progress monitoring:

```cpp
class TrainingObserver {
public:
    virtual ~TrainingObserver() = default;
    virtual void onEpochStart(int epoch) = 0;
    virtual void onEpochEnd(int epoch, float loss, float accuracy) = 0;
    virtual void onBatchProcessed(size_t batch, size_t totalBatches) = 0;
    virtual void onTrainingComplete(const TrainingResults& results) = 0;
};

class TrainingManager {
private:
    std::vector<std::unique_ptr<TrainingObserver>> observers_;

public:
    void addObserver(std::unique_ptr<TrainingObserver> observer) {
        observers_.push_back(std::move(observer));
    }

    void notifyEpochEnd(int epoch, float loss, float accuracy) {
        for (auto& observer : observers_) {
            observer->onEpochEnd(epoch, loss, accuracy);
        }
    }
};
```

### 4. Factory Pattern

Used for creating different types of neural network components:

```cpp
template<typename T>
class LayerFactory {
public:
    static std::unique_ptr<Layer<T>> createLayer(
        LayerType type,
        size_t inputSize,
        size_t outputSize,
        const LayerConfig& config
    ) {
        switch (type) {
            case LayerType::DENSE:
                return std::make_unique<DenseLayer<T>>(inputSize, outputSize, config);
            case LayerType::CONVOLUTIONAL:
                return std::make_unique<ConvolutionalLayer<T>>(inputSize, outputSize, config);
            case LayerType::RECURRENT:
                return std::make_unique<RecurrentLayer<T>>(inputSize, outputSize, config);
            default:
                throw std::invalid_argument("Unknown layer type");
        }
    }
};
```

### 5. Command Pattern

Used for undo/redo functionality:

```cpp
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual bool canUndo() const = 0;
    virtual std::string getDescription() const = 0;
};

class ChangeNetworkStructureCommand : public Command {
private:
    NeuralNetwork<float>* network_;
    NetworkConfig oldConfig_;
    NetworkConfig newConfig_;

public:
    ChangeNetworkStructureCommand(NeuralNetwork<float>* network,
                                const NetworkConfig& newConfig);

    void execute() override;
    void undo() override;
    bool canUndo() const override { return true; }
    std::string getDescription() const override {
        return "Change network structure";
    }
};

class CommandHistory {
private:
    std::vector<std::unique_ptr<Command>> history_;
    size_t currentIndex_ = 0;

public:
    void executeCommand(std::unique_ptr<Command> command);
    void undo();
    void redo();
    bool canUndo() const;
    bool canRedo() const;
};
```

---

## Memory Management

### RAII (Resource Acquisition Is Initialization)

All resources are managed through RAII principles:

```cpp
class NetworkRenderer {
private:
    std::unique_ptr<sf::RenderWindow> window_;     // Automatically cleaned up
    std::vector<sf::Texture> textures_;           // Vector manages memory
    std::unique_ptr<sf::Font> font_;              // Smart pointer manages lifetime

public:
    NetworkRenderer(int width, int height)
        : window_(std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "NN Visualizer"))
        , font_(std::make_unique<sf::Font>()) {

        if (!font_->loadFromFile("assets/fonts/roboto.ttf")) {
            throw std::runtime_error("Failed to load font");
        }
    }

    // Destructor automatically called, all resources cleaned up
    ~NetworkRenderer() = default;
};
```

### Smart Pointers Usage Guidelines

```cpp
// Use unique_ptr for exclusive ownership
std::unique_ptr<NeuralNetwork<float>> network_;

// Use shared_ptr for shared ownership (use sparingly)
std::shared_ptr<TrainingData> sharedData_;

// Use weak_ptr to break circular dependencies
std::weak_ptr<TrainingObserver> observer_;

// Prefer stack allocation when possible
NetworkConfig config;  // Not NetworkConfig* config = new NetworkConfig();

// Use containers instead of raw arrays
std::vector<float> weights_;  // Not float* weights_;
```

### Memory Pool for Performance

For high-performance scenarios, we use memory pools:

```cpp
template<typename T>
class MemoryPool {
private:
    std::vector<T> pool_;
    std::queue<T*> available_;
    size_t poolSize_;

public:
    explicit MemoryPool(size_t size) : poolSize_(size) {
        pool_.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            pool_.emplace_back();
            available_.push(&pool_[i]);
        }
    }

    T* acquire() {
        if (available_.empty()) {
            throw std::runtime_error("Memory pool exhausted");
        }
        T* ptr = available_.front();
        available_.pop();
        return ptr;
    }

    void release(T* ptr) {
        // Reset object state if needed
        *ptr = T{};
        available_.push(ptr);
    }
};

// Usage in training loop
class BatchProcessor {
private:
    MemoryPool<std::vector<float>> vectorPool_{1000};

public:
    void processBatch(const std::vector<std::vector<float>>& batch) {
        auto* tempVector = vectorPool_.acquire();

        // Use tempVector for computations
        // ...

        vectorPool_.release(tempVector);
    }
};
```

---

## Threading Model

### Main Thread Structure

```
Main Thread
├── UI Event Loop (60 FPS)
├── Rendering Loop (60 FPS)
└── Background Tasks Manager
```

### Worker Threads

```cpp
class ThreadPool {
private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    bool stop_;

public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
};

// Usage for parallel training
class ParallelTrainer {
private:
    ThreadPool pool_;

public:
    void trainBatchParallel(NeuralNetwork<float>& network,
                          const std::vector<TrainingData>& batch) {
        const size_t numThreads = std::thread::hardware_concurrency();
        const size_t batchSize = batch.size() / numThreads;

        std::vector<std::future<void>> futures;

        for (size_t i = 0; i < numThreads; ++i) {
            size_t start = i * batchSize;
            size_t end = (i == numThreads - 1) ? batch.size() : (i + 1) * batchSize;

            futures.push_back(pool_.enqueue([&network, &batch, start, end]() {
                for (size_t j = start; j < end; ++j) {
                    network.train(batch[j].input, batch[j].target);
                }
            }));
        }

        // Wait for all threads to complete
        for (auto& future : futures) {
            future.wait();
        }
    }
};
```

### Thread Safety

Critical sections are protected with appropriate synchronization:

```cpp
class ThreadSafeNetworkStats {
private:
    mutable std::mutex mutex_;
    NetworkStats stats_;

public:
    void updateLoss(float newLoss) {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_.currentLoss = newLoss;
        stats_.averageLoss = (stats_.averageLoss * stats_.iterations + newLoss)
                            / (stats_.iterations + 1);
        ++stats_.iterations;
    }

    NetworkStats getStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return stats_;  // Copy returned, safe to access
    }
};
```

---

## Plugin Architecture

The system supports plugins for extensibility:

```cpp
class IPlugin {
public:
    virtual ~IPlugin() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual std::vector<std::string> getDependencies() const = 0;
};

class PluginManager {
private:
    std::vector<std::unique_ptr<IPlugin>> loadedPlugins_;
    std::unordered_map<std::string, std::function<std::unique_ptr<IPlugin>()>> factories_;

public:
    bool loadPlugin(const std::string& pluginPath);
    void unloadPlugin(const std::string& pluginName);
    std::vector<std::string> getLoadedPlugins() const;

    template<typename T>
    void registerPlugin(const std::string& name) {
        factories_[name] = []() { return std::make_unique<T>(); };
    }
};

// Example plugin for custom activation function
class CustomActivationPlugin : public IPlugin {
public:
    std::string getName() const override { return "CustomActivation"; }
    std::string getVersion() const override { return "1.0.0"; }

    bool initialize() override {
        // Register custom activation functions
        ActivationRegistry::registerFunction("swish",
            [](float x) { return x / (1.0f + std::exp(-x)); },
            [](float x) {
                float sigmoid = 1.0f / (1.0f + std::exp(-x));
                return sigmoid + x * sigmoid * (1.0f - sigmoid);
            });
        return true;
    }

    void shutdown() override {
        ActivationRegistry::unregisterFunction("swish");
    }

    std::vector<std::string> getDependencies() const override {
        return {"core"};
    }
};
```

---

## Performance Considerations

### SIMD Optimizations

Critical mathematical operations use SIMD when available:

```cpp
#ifdef USE_AVX2
#include <immintrin.h>

void vectorMultiplyAVX2(const float* a, const float* b, float* result, size_t size) {
    const size_t simdSize = 8;  // AVX2 processes 8 floats at once
    size_t i = 0;

    // Process 8 elements at a time
    for (; i + simdSize <= size; i += simdSize) {
        __m256 va = _mm256_load_ps(&a[i]);
        __m256 vb = _mm256_load_ps(&b[i]);
        __m256 vresult = _mm256_mul_ps(va, vb);
        _mm256_store_ps(&result[i], vresult);
    }

    // Handle remaining elements
    for (; i < size; ++i) {
        result[i] = a[i] * b[i];
    }
}
#endif

class SIMDMathOperations {
public:
    static void vectorMultiply(const float* a, const float* b, float* result, size_t size) {
#ifdef USE_AVX2
        vectorMultiplyAVX2(a, b, result, size);
#else
        for (size_t i = 0; i < size; ++i) {
            result[i] = a[i] * b[i];
        }
#endif
    }
};
```

### Memory Layout Optimization

Data structures are organized for cache efficiency:

```cpp
// Structure of Arrays (SoA) for better cache locality
class OptimizedLayer {
private:
    // Instead of Array of Structures
    // std::vector<Neuron> neurons_;

    // Use Structure of Arrays
    std::vector<float> weights_;     // All weights contiguous
    std::vector<float> biases_;      // All biases contiguous
    std::vector<float> outputs_;     // All outputs contiguous
    std::vector<float> gradients_;   // All gradients contiguous

    size_t neuronCount_;
    size_t weightsPerNeuron_;

public:
    void forward(const std::vector<float>& inputs) {
        // Process all neurons in tight loops for better cache usage
        for (size_t neuron = 0; neuron < neuronCount_; ++neuron) {
            float sum = biases_[neuron];

            for (size_t weight = 0; weight < weightsPerNeuron_; ++weight) {
                sum += inputs[weight] * weights_[neuron * weightsPerNeuron_ + weight];
            }

            outputs_[neuron] = activationFunction(sum);
        }
    }
};
```

### Profiling Integration

Built-in profiling for performance monitoring:

```cpp
class Profiler {
private:
    struct ProfileData {
        std::chrono::high_resolution_clock::time_point start;
        std::chrono::duration<double, std::milli> accumulated{0};
        size_t callCount = 0;
    };

    std::unordered_map<std::string, ProfileData> profiles_;
    mutable std::mutex mutex_;

public:
    class ScopedTimer {
    private:
        Profiler& profiler_;
        std::string name_;
        std::chrono::high_resolution_clock::time_point start_;

    public:
        ScopedTimer(Profiler& profiler, const std::string& name)
            : profiler_(profiler), name_(name)
            , start_(std::chrono::high_resolution_clock::now()) {}

        ~ScopedTimer() {
            auto end = std::chrono::high_resolution_clock::now();
            profiler_.addSample(name_, end - start_);
        }
    };

    void addSample(const std::string& name,
                   const std::chrono::duration<double, std::milli>& duration) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto& data = profiles_[name];
        data.accumulated += duration;
        ++data.callCount;
    }

    ScopedTimer createTimer(const std::string& name) {
        return ScopedTimer(*this, name);
    }

    void printReport() const {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& [name, data] : profiles_) {
            std::cout << name << ": "
                     << data.accumulated.count() << "ms total, "
                     << data.callCount << " calls, "
                     << data.accumulated.count() / data.callCount << "ms avg"
                     << std::endl;
        }
    }
};

// Usage macro for easy profiling
#define PROFILE_SCOPE(profiler, name) auto _timer = profiler.createTimer(name)

// Example usage in neural network forward pass
void NeuralNetwork::forward(const std::vector<float>& input) {
    PROFILE_SCOPE(profiler_, "NeuralNetwork::forward");

    for (auto& layer : layers_) {
        PROFILE_SCOPE(profiler_, "Layer::forward");
        input = layer->forward(input);
    }
}
```

---

## Future Extensibility

### Plugin System Design

The architecture supports future extensions through well-defined interfaces:

```cpp
// Extension points for new layer types
class ILayerExtension {
public:
    virtual std::string getLayerType() const = 0;
    virtual std::unique_ptr<Layer<float>> createLayer(const LayerConfig& config) = 0;
    virtual bool supportsGPU() const = 0;
    virtual std::vector<ParameterInfo> getParameters() const = 0;
};

// Extension points for new optimizers
class IOptimizerExtension {
public:
    virtual std::string getOptimizerName() const = 0;
    virtual std::unique_ptr<Optimizer<float>> createOptimizer(const OptimizerConfig& config) = 0;
    virtual std::vector<ParameterInfo> getHyperparameters() const = 0;
};

// Extension points for new data formats
class IDataLoaderExtension {
public:
    virtual std::vector<std::string> getSupportedFormats() const = 0;
    virtual std::unique_ptr<DataSet> loadData(const std::string& path) = 0;
    virtual bool canSave() const = 0;
    virtual void saveData(const DataSet& dataset, const std::string& path) = 0;
};
```

### API Versioning

Future API changes are handled through versioning:

```cpp
namespace nnv::api::v1 {
    // Version 1 API
    class NeuralNetwork { /* ... */ };
}

namespace nnv::api::v2 {
    // Version 2 API with backwards compatibility
    class NeuralNetwork : public v1::NeuralNetwork { /* ... */ };
}

// Type alias for current version
namespace nnv {
    using NeuralNetwork = api::v2::NeuralNetwork;
}
```

### Configuration System

Extensible configuration system for future features:

```cpp
class ConfigurationSchema {
public:
    void defineSection(const std::string& section);
    void defineParameter(const std::string& section,
                        const std::string& name,
                        ParameterType type,
                        const std::any& defaultValue,
                        const std::string& description);

    bool validateConfiguration(const Configuration& config) const;
    Configuration getDefaults() const;
};

// Usage for extending configuration
class GPUExtension {
public:
    void registerConfiguration(ConfigurationSchema& schema) {
        schema.defineSection("gpu");
        schema.defineParameter("gpu", "enable", ParameterType::BOOLEAN,
                             false, "Enable GPU acceleration");
        schema.defineParameter("gpu", "device_id", ParameterType::INTEGER,
                             0, "GPU device ID to use");
        schema.defineParameter("gpu", "memory_limit", ParameterType::FLOAT,
                             0.8f, "Maximum GPU memory usage (0.0-1.0)");
    }
};
```

---

This architecture provides a solid foundation for the Neural Network Visualizer while maintaining flexibility for future enhancements. The modular design, clear interfaces, and extensibility points ensure that the system can grow and adapt to new requirements while maintaining code quality and performance.