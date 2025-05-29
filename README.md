# Neural Network Visualizer

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://github.com/keshav/nnVisualiser/actions/workflows/ci.yml/badge.svg)](https://github.com/keshav/nnVisualiser/actions)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![vcpkg](https://img.shields.io/badge/package%20manager-vcpkg-green)](https://vcpkg.io/)

A high-performance, interactive neural network visualization application built in C++17. This project provides real-time visualization of neural network training, architecture exploration, and educational tools for understanding deep learning concepts.

![Neural Network Visualizer Demo](docs/images/demo.gif)

## 🌟 Features

### Core Neural Network Engine
- **Custom Neural Network Implementation**: Pure C++ implementation with support for various architectures
- **Multiple Activation Functions**: Sigmoid, ReLU, Tanh, Leaky ReLU, and more
- **Flexible Loss Functions**: MSE, Cross-entropy, and custom loss implementations
- **Advanced Weight Initialization**: Xavier, He, and custom initialization strategies
- **Optimizers**: SGD, Adam, RMSprop with momentum support

### Visualization & Graphics
- **Real-time Training Visualization**: Watch neural networks learn in real-time
- **Interactive Network Architecture**: Drag, modify, and explore network structures
- **Beautiful Animations**: Smooth transitions showing data flow through networks
- **Customizable Color Schemes**: Multiple themes for different visualization preferences
- **Performance Monitoring**: Real-time performance metrics and training statistics

### User Interface
- **Modern GUI**: Clean, intuitive interface built with SFML and ImGui
- **Drag & Drop Network Builder**: Visual network construction
- **Configuration Management**: Save and load network configurations
- **Export Capabilities**: Export trained models and visualization screenshots

### Educational Tools
- **Interactive Tutorials**: Built-in tutorials for neural network concepts
- **Example Datasets**: Pre-loaded datasets (XOR, MNIST digits, etc.)
- **Parameter Exploration**: Real-time parameter adjustment with immediate feedback
- **Layer Analysis**: Detailed inspection of individual layers and neurons

## 🚀 Quick Start

### Prerequisites

- **C++17** compatible compiler (GCC 7+, Clang 6+, MSVC 2017+)
- **CMake** 3.16 or higher
- **vcpkg** package manager
- **Git** for cloning the repository

### Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/keshav/nnVisualiser.git
   cd nnVisualiser
   ```

2. **Install system dependencies** (Ubuntu/Debian):
   ```bash
   sudo apt-get install libx11-dev libxi-dev libxrandr-dev libxcursor-dev libudev-dev libgl1-mesa-dev
   ```

3. **Set up vcpkg**:
   ```bash
   export VCPKG_ROOT=/path/to/your/vcpkg
   ```

4. **Build the project**:
   ```bash
   cmake --preset vcpkg
   cmake --build build/vcpkg --config Release
   ```

5. **Run the application**:
   ```bash
   ./build/vcpkg/src/NeuralNetworkVisualizer
   ```

### Quick Example

```cpp
#include "core/NeuralNetwork.hpp"
#include "core/Types.hpp"

using namespace nnv::core;

int main() {
    // Create a simple XOR network
    NetworkConfig config;
    config.inputSize = 2;
    config.hiddenLayers = {4, 4};
    config.outputSize = 1;
    config.activationFunction = ActivationType::RELU;
    config.learningRate = 0.01f;

    auto network = std::make_unique<NeuralNetwork<float>>(config);

    // Training data for XOR problem
    std::vector<std::vector<float>> inputs = {{0,0}, {0,1}, {1,0}, {1,1}};
    std::vector<std::vector<float>> targets = {{0}, {1}, {1}, {0}};

    // Train the network
    for (int epoch = 0; epoch < 1000; ++epoch) {
        for (size_t i = 0; i < inputs.size(); ++i) {
            network->train(inputs[i], targets[i]);
        }
    }

    // Test the network
    for (size_t i = 0; i < inputs.size(); ++i) {
        auto output = network->predict(inputs[i]);
        std::cout << "Input: [" << inputs[i][0] << ", " << inputs[i][1]
                  << "] -> Output: " << output[0] << std::endl;
    }

    return 0;
}
```

## 📖 Documentation

### User Guides
- [Installation Guide](docs/guides/installation.md) - Detailed setup instructions
- [Getting Started](docs/guides/getting-started.md) - First steps with the application
- [User Interface Guide](docs/guides/ui-guide.md) - Complete UI reference
- [Configuration Guide](docs/guides/configuration.md) - Customization options

### Tutorials
- [Building Your First Network](docs/tutorials/first-network.md)
- [Understanding Backpropagation](docs/tutorials/backpropagation.md)
- [Advanced Network Architectures](docs/tutorials/advanced-architectures.md)
- [Custom Loss Functions](docs/tutorials/custom-loss-functions.md)

### API Reference
- [Core API](docs/api/core.md) - Neural network engine
- [Graphics API](docs/api/graphics.md) - Visualization system
- [Utils API](docs/api/utils.md) - Utility functions
- [UI API](docs/api/ui.md) - User interface components

### Developer Guides
- [Architecture Overview](docs/guides/architecture.md) - System design
- [Building from Source](docs/guides/building.md) - Development setup
- [Contributing Guide](docs/guides/contributing.md) - How to contribute
- [Testing Guide](docs/guides/testing.md) - Running and writing tests

## 🏗️ Architecture

The Neural Network Visualizer is built with a modular architecture:

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Application   │────│   UI System     │────│   Graphics      │
│     Layer       │    │                 │    │     Engine      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         └───────────────────────┼───────────────────────┘
                                 │
                    ┌─────────────────┐
                    │   Neural Net    │
                    │     Engine      │
                    └─────────────────┘
                             │
                    ┌─────────────────┐
                    │   Utilities     │
                    │   & Helpers     │
                    └─────────────────┘
```

### Key Components

- **Core Engine**: High-performance neural network implementation
- **Graphics System**: Real-time visualization and rendering
- **UI Framework**: Interactive interface built with ImGui
- **Utilities**: Configuration, logging, and data management

## 🧪 Examples

The `examples/` directory contains various demonstrations:

- **simple_xor_demo.cpp**: Basic XOR problem solver
- **complete_demo.cpp**: Full-featured demonstration
- **mnist_classifier**: Handwritten digit recognition
- **custom_network**: Advanced network customization

### Running Examples

```bash
# Build examples
cmake --build build/vcpkg --target examples

# Run XOR demo
./build/vcpkg/examples/simple_xor_demo

# Run complete demo
./build/vcpkg/examples/complete_demo
```

## 🛠️ Development

### Project Structure

```
nnVisualiser/
├── src/                    # Source code
│   ├── core/              # Neural network engine
│   ├── graphics/          # Visualization system
│   ├── ui/                # User interface
│   ├── utils/             # Utilities
│   └── main.cpp           # Application entry point
├── include/               # Header files
├── examples/              # Example applications
├── tests/                 # Unit tests
├── docs/                  # Documentation
├── external/              # Third-party libraries
└── build/                 # Build output
```

### Building for Development

```bash
# Debug build
cmake --preset debug
cmake --build build/debug

# Run tests
ctest --test-dir build/debug
```

### Code Style

This project follows modern C++ best practices:

- C++17 standard
- RAII principles
- Smart pointers for memory management
- Exception-safe code
- Comprehensive unit testing

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](docs/guides/contributing.md) for details on:

- Code of conduct
- Development workflow
- Coding standards
- Testing requirements
- Documentation guidelines

### Quick Contribution Steps

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📊 Performance

The Neural Network Visualizer is optimized for performance:

- **SIMD Optimizations**: AVX2 support for matrix operations
- **Multi-threading**: Parallel training and rendering
- **Memory Efficiency**: Optimized memory layout and allocation
- **Real-time Rendering**: 60+ FPS visualization during training

### Benchmarks

| Operation | Time (ms) | Throughput |
|-----------|-----------|------------|
| Forward Pass (1000 neurons) | 0.15 | 6,666 ops/sec |
| Backward Pass (1000 neurons) | 0.28 | 3,571 ops/sec |
| Matrix Multiplication (512x512) | 2.1 | 476 ops/sec |

*Benchmarks run on Intel i7-9700K, compiled with GCC 9.3.0, -O3 optimization*

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Eigen3**: Linear algebra library
- **SFML**: Multimedia framework
- **ImGui**: Immediate mode GUI
- **nlohmann/json**: JSON parsing
- **spdlog**: Fast logging library
- **vcpkg**: Package management

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/keshav/nnVisualiser/issues)
- **Discussions**: [GitHub Discussions](https://github.com/keshav/nnVisualiser/discussions)
- **Email**: support@nnvisualizer.dev
- **Documentation**: [Full Documentation](https://nnvisualizer.dev/docs)

## 🗺️ Roadmap

### Version 2.0 (Upcoming)
- [ ] GPU acceleration with CUDA/OpenCL
- [ ] Advanced network architectures (CNN, RNN, LSTM)
- [ ] Distributed training support
- [ ] Python API bindings
- [ ] Web-based interface

### Version 1.1
- [ ] Additional activation functions
- [ ] Custom dataset loading
- [ ] Model export formats (ONNX, TensorFlow)
- [ ] Enhanced visualization options

See our [full roadmap](docs/guides/roadmap.md) for more details.

---

<div align="center">

**Built with ❤️ by the Neural Network Visualizer Team**

[Website](https://nnvisualizer.dev) • [Documentation](https://nnvisualizer.dev/docs) • [API Reference](https://nnvisualizer.dev/api) • [Examples](https://nnvisualizer.dev/examples)

</div>