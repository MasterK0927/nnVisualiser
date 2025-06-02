# Contributing Guide

We're excited that you're interested in contributing to the Neural Network Visualizer! This guide will help you get started and understand our development workflow.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Types of Contributions](#types-of-contributions)
- [Development Workflow](#development-workflow)
- [Coding Standards](#coding-standards)
- [Testing Requirements](#testing-requirements)
- [Documentation Guidelines](#documentation-guidelines)
- [Submitting Changes](#submitting-changes)
- [Community and Communication](#community-and-communication)

---

## Code of Conduct

### Our Pledge

We pledge to make participation in our project a harassment-free experience for everyone, regardless of age, body size, disability, ethnicity, sex characteristics, gender identity and expression, level of experience, education, socio-economic status, nationality, personal appearance, race, religion, or sexual identity and orientation.

### Our Standards

**Positive behaviors include:**

- Using welcoming and inclusive language
- Being respectful of differing viewpoints and experiences
- Gracefully accepting constructive criticism
- Focusing on what is best for the community
- Showing empathy towards other community members

**Unacceptable behaviors include:**

- Harassment, discriminatory comments, or personal attacks
- Public or private harassment
- Publishing others' private information without permission
- Other conduct that could reasonably be considered inappropriate

### Enforcement

Project maintainers have the right to remove, edit, or reject contributions that don't align with this Code of Conduct. Report unacceptable behavior to [conduct@nnvisualizer.dev](mailto:conduct@nnvisualizer.dev).

---

## Getting Started

### Prerequisites

1. **Development Environment**: See our [Installation Guide](installation.md)
2. **Git Knowledge**: Basic understanding of Git workflows
3. **C++17**: Familiarity with modern C++ practices
4. **CMake**: Understanding of build systems

### Fork and Clone

```bash
# Fork the repository on GitHub, then clone your fork
git clone https://github.com/YOUR_USERNAME/nnVisualiser.git
cd nnVisualiser

# Add the upstream remote
git remote add upstream https://github.com/keshav/nnVisualiser.git

# Verify remotes
git remote -v
```

### Set Up Development Environment

```bash
# Install development dependencies
sudo apt-get install -y clang-format valgrind cppcheck doxygen

# Set up pre-commit hooks
cp .githooks/pre-commit .git/hooks/
chmod +x .git/hooks/pre-commit

# Build in debug mode
cmake --preset debug
cmake --build build/debug

# Run tests to ensure everything works
ctest --test-dir build/debug --verbose
```

---

## Types of Contributions

We welcome various types of contributions:

### 🐛 Bug Reports

**Before creating a bug report:**
- Check if it's already reported in [GitHub Issues](https://github.com/keshav/nnVisualiser/issues)
- Try the latest version
- Check if it's actually a bug, not expected behavior

**Good bug reports include:**
- Clear, descriptive title
- Steps to reproduce
- Expected vs actual behavior
- Environment details (OS, compiler, versions)
- Minimal code example (if applicable)
- Screenshots/videos for UI issues

**Bug report template:**
```markdown
**Describe the bug**
A clear description of what the bug is.

**To Reproduce**
Steps to reproduce the behavior:
1. Go to '...'
2. Click on '....'
3. See error

**Expected behavior**
What you expected to happen.

**Environment:**
- OS: [e.g. Ubuntu 20.04]
- Compiler: [e.g. GCC 9.3.0]
- Version: [e.g. v1.2.0]

**Additional context**
Any other context about the problem.
```

### ✨ Feature Requests

**Before requesting a feature:**
- Check existing feature requests
- Consider if it fits the project scope
- Think about implementation complexity

**Good feature requests include:**
- Clear problem statement
- Proposed solution
- Alternative solutions considered
- Implementation ideas (optional)

### 🔧 Code Contributions

**Areas where we need help:**
- Core neural network algorithms
- Visualization improvements
- Performance optimizations
- Platform-specific fixes
- Test coverage
- Documentation

**Priority areas:**
- GPU acceleration support
- Additional neural network layers
- Export/import functionality
- Mobile platform support

### 📚 Documentation

**Documentation needs:**
- API documentation improvements
- Tutorial content
- Example programs
- Translation to other languages
- Video tutorials

---

## Development Workflow

### 1. Choose or Create an Issue

- Browse [good first issues](https://github.com/keshav/nnVisualiser/labels/good%20first%20issue)
- Comment on the issue to claim it
- Get approval from maintainers for large changes

### 2. Create a Feature Branch

```bash
# Make sure you're on main and up to date
git checkout main
git pull upstream main

# Create a new branch for your feature
git checkout -b feature/your-feature-name

# Or for bug fixes
git checkout -b fix/issue-123-brief-description
```

### 3. Make Your Changes

Follow our [coding standards](#coding-standards) and:

- Keep changes focused and atomic
- Write clear, descriptive commit messages
- Add tests for new functionality
- Update documentation as needed

### 4. Test Your Changes

```bash
# Build and run tests
cmake --build build/debug
ctest --test-dir build/debug

# Run static analysis
cppcheck --enable=all --std=c++17 src/

# Check formatting
clang-format -i src/**/*.cpp include/**/*.hpp

# Run sanitizers (optional but recommended)
cmake --preset debug -DCMAKE_CXX_FLAGS="-fsanitize=address -fsanitize=undefined"
cmake --build build/debug
ctest --test-dir build/debug
```

### 5. Commit Your Changes

We use [Conventional Commits](https://www.conventionalcommits.org/):

```bash
# Format: <type>(<scope>): <description>
git commit -m "feat(core): add support for LSTM layers"
git commit -m "fix(ui): resolve crash when loading large networks"
git commit -m "docs(api): improve NeuralNetwork class documentation"
git commit -m "test(core): add unit tests for activation functions"
```

**Commit types:**
- `feat`: New features
- `fix`: Bug fixes
- `docs`: Documentation changes
- `style`: Code style changes (formatting, etc.)
- `refactor`: Code refactoring
- `perf`: Performance improvements
- `test`: Test changes
- `build`: Build system changes
- `ci`: CI/CD changes

---

## Coding Standards

### C++ Style Guidelines

We follow a modified version of the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

#### Naming Conventions

```cpp
// Classes: PascalCase
class NeuralNetwork {
public:
    // Public methods: camelCase
    void trainNetwork();
    float getLearningRate() const;

private:
    // Private members: camelCase with underscore
    float learningRate_;
    std::vector<Layer> layers_;
};

// Namespaces: lowercase
namespace nnv {
namespace core {
    // Functions: camelCase
    float calculateError(const std::vector<float>& predicted,
                        const std::vector<float>& actual);

    // Constants: UPPER_CASE
    const float DEFAULT_LEARNING_RATE = 0.01f;

    // Enums: PascalCase with UPPER_CASE values
    enum class ActivationType {
        RELU,
        SIGMOID,
        TANH
    };
}
}
```

#### File Organization

```cpp
// Header files (.hpp)
#pragma once

// System includes first
#include <vector>
#include <memory>
#include <string>

// Third-party includes
#include <eigen3/Eigen/Dense>

// Project includes
#include "core/Types.hpp"
#include "utils/Common.hpp"

namespace nnv {
namespace core {

class NeuralNetwork {
    // Public interface first
public:
    explicit NeuralNetwork(const NetworkConfig& config);
    ~NeuralNetwork() = default;

    // Copy/move semantics
    NeuralNetwork(const NeuralNetwork&) = delete;
    NeuralNetwork& operator=(const NeuralNetwork&) = delete;
    NeuralNetwork(NeuralNetwork&&) = default;
    NeuralNetwork& operator=(NeuralNetwork&&) = default;

    // Main interface
    void train(const std::vector<float>& input,
               const std::vector<float>& target);
    std::vector<float> predict(const std::vector<float>& input);

private:
    // Private members last
    std::vector<Layer> layers_;
    float learningRate_;
    LossType lossFunction_;
};

} // namespace core
} // namespace nnv
```

#### Code Style Rules

```cpp
// Use auto judiciously
auto network = std::make_unique<NeuralNetwork>(config); // Good
auto x = 42; // Avoid - type not obvious

// Prefer range-based for loops
for (const auto& layer : layers_) {
    layer.process();
}

// Use meaningful variable names
float learningRate = 0.01f; // Good
float lr = 0.01f;          // Avoid

// Initialize variables at declaration
std::vector<float> weights(neuronCount, 0.0f); // Good
std::vector<float> weights;                     // Avoid
weights.resize(neuronCount);

// Use const whenever possible
const auto& getWeights() const { return weights_; } // Good
void processLayer(const Layer& layer);              // Good

// RAII for resource management
class NetworkLoader {
public:
    explicit NetworkLoader(const std::string& filename)
        : file_(filename) {
        if (!file_.is_open()) {
            throw std::runtime_error("Cannot open file");
        }
    }

private:
    std::ifstream file_;
};
```

### Performance Guidelines

```cpp
// Pass large objects by const reference
void trainBatch(const std::vector<TrainingData>& data); // Good
void trainBatch(std::vector<TrainingData> data);        // Avoid copying

// Use move semantics for expensive operations
std::vector<float> createLargeVector() {
    std::vector<float> result(1000000);
    // ... populate result
    return result; // Automatic move in C++17
}

// Reserve vector capacity when size is known
std::vector<float> results;
results.reserve(expectedSize);

// Use emplace instead of push when constructing in-place
neurons_.emplace_back(inputSize, ActivationType::RELU);
// Instead of:
// neurons_.push_back(Neuron(inputSize, ActivationType::RELU));
```

---

## Testing Requirements

### Unit Tests

We use Google Test (GTest) for unit testing:

```cpp
// tests/core/test_neural_network.cpp
#include <gtest/gtest.h>
#include "core/NeuralNetwork.hpp"

class NeuralNetworkTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_.inputSize = 2;
        config_.hiddenLayers = {4};
        config_.outputSize = 1;
        config_.activationFunction = ActivationType::SIGMOID;
        config_.learningRate = 0.1f;
    }

    NetworkConfig config_;
};

TEST_F(NeuralNetworkTest, CreatesNetworkWithCorrectSize) {
    auto network = std::make_unique<NeuralNetwork<float>>(config_);
    EXPECT_EQ(network->getLayerCount(), 3); // input + hidden + output
}

TEST_F(NeuralNetworkTest, TrainsOnXORProblem) {
    auto network = std::make_unique<NeuralNetwork<float>>(config_);

    // XOR training data
    std::vector<std::vector<float>> inputs = {{0,0}, {0,1}, {1,0}, {1,1}};
    std::vector<std::vector<float>> targets = {{0}, {1}, {1}, {0}};

    // Train for a reasonable number of epochs
    for (int epoch = 0; epoch < 5000; ++epoch) {
        for (size_t i = 0; i < inputs.size(); ++i) {
            network->train(inputs[i], targets[i]);
        }
    }

    // Test accuracy
    for (size_t i = 0; i < inputs.size(); ++i) {
        auto output = network->predict(inputs[i]);
        float error = std::abs(output[0] - targets[i][0]);
        EXPECT_LT(error, 0.1f) << "Failed for input [" << inputs[i][0]
                               << ", " << inputs[i][1] << "]";
    }
}

// Test edge cases
TEST_F(NeuralNetworkTest, HandlesInvalidConfiguration) {
    config_.inputSize = 0; // Invalid
    EXPECT_THROW(NeuralNetwork<float>(config_), InvalidConfigException);
}
```

### Integration Tests

```cpp
// tests/integration/test_full_workflow.cpp
TEST(IntegrationTest, CompleteTrainingWorkflow) {
    // Test the complete workflow from data loading to model saving
    auto dataLoader = std::make_unique<DataLoader>();
    auto dataset = dataLoader->loadXORData();

    NetworkConfig config;
    // ... configure network

    auto network = std::make_unique<NeuralNetwork<float>>(config);

    // Train
    for (const auto& sample : dataset) {
        network->train(sample.input, sample.target);
    }

    // Save model
    network->saveModel("test_model.json");

    // Load model and verify
    auto loadedNetwork = std::make_unique<NeuralNetwork<float>>(config);
    loadedNetwork->loadModel("test_model.json");

    // Test that loaded model produces same results
    auto originalOutput = network->predict({0.5f, 0.5f});
    auto loadedOutput = loadedNetwork->predict({0.5f, 0.5f});

    EXPECT_NEAR(originalOutput[0], loadedOutput[0], 1e-6f);
}
```

### Running Tests

```bash
# Build tests
cmake --build build/debug --target tests

# Run all tests
ctest --test-dir build/debug --verbose

# Run specific test suite
ctest --test-dir build/debug --verbose -R "NeuralNetworkTest"

# Run with memory checking
valgrind --tool=memcheck --leak-check=full ./build/debug/tests/test_core

# Generate coverage report
cmake --preset debug -DCMAKE_CXX_FLAGS="--coverage"
cmake --build build/debug
ctest --test-dir build/debug
gcovr -r . --html --html-details -o coverage.html
```

---

## Documentation Guidelines

### Code Documentation

Use Doxygen-style comments:

```cpp
/**
 * @brief Creates and trains a neural network for the specified task.
 *
 * This function creates a neural network with the given configuration,
 * trains it on the provided dataset, and returns the trained network.
 *
 * @param config Network configuration specifying architecture and training parameters
 * @param dataset Training dataset containing input-target pairs
 * @param maxEpochs Maximum number of training epochs (default: 1000)
 * @param targetError Target error for early stopping (default: 0.01)
 *
 * @return std::unique_ptr<NeuralNetwork<float>> Trained neural network
 *
 * @throws InvalidConfigException If the network configuration is invalid
 * @throws DataException If the dataset is empty or malformed
 *
 * @example
 * @code
 * NetworkConfig config;
 * config.inputSize = 2;
 * config.hiddenLayers = {4, 3};
 * config.outputSize = 1;
 *
 * auto dataset = loadXORDataset();
 * auto network = trainNetwork(config, dataset, 5000, 0.001f);
 * @endcode
 *
 * @see NetworkConfig
 * @see TrainingDataset
 * @since version 1.0
 */
std::unique_ptr<NeuralNetwork<float>> trainNetwork(
    const NetworkConfig& config,
    const TrainingDataset& dataset,
    int maxEpochs = 1000,
    float targetError = 0.01f
);
```

### README and Markdown Guidelines

- Use clear, descriptive headings
- Include code examples with expected output
- Add screenshots for UI features
- Keep line length reasonable (80-100 characters)
- Use tables for structured information
- Include links to related documentation

---

## Submitting Changes

### Pull Request Process

1. **Ensure CI passes**: All tests and checks must pass
2. **Update documentation**: Include relevant docs updates
3. **Add changelog entry**: Update CHANGELOG.md
4. **Request review**: Add appropriate reviewers

### Pull Request Template

```markdown
## Description
Brief description of changes and motivation.

Fixes #(issue number)

## Type of Change
- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Documentation update

## Testing
- [ ] I have added tests that prove my fix is effective or that my feature works
- [ ] New and existing unit tests pass locally with my changes
- [ ] I have tested this change on multiple platforms (if applicable)

## Checklist
- [ ] My code follows the project's coding standards
- [ ] I have performed a self-review of my code
- [ ] I have commented my code, particularly in hard-to-understand areas
- [ ] I have made corresponding changes to the documentation
- [ ] My changes generate no new warnings
- [ ] Any dependent changes have been merged and published
```

### Review Process

**For Contributors:**
- Be responsive to feedback
- Make requested changes promptly
- Ask questions if feedback is unclear
- Keep discussions professional and constructive

**For Reviewers:**
- Provide specific, actionable feedback
- Explain the reasoning behind suggestions
- Be respectful and encouraging
- Focus on code quality, not personal style preferences

---

## Community and Communication

### Getting Help

- **Discord**: Join our [Discord server](https://discord.gg/nnvisualizer) for real-time chat
- **GitHub Discussions**: Use [GitHub Discussions](https://github.com/keshav/nnVisualiser/discussions) for questions
- **Stack Overflow**: Tag questions with `neural-network-visualizer`

### Regular Meetings

- **Weekly Dev Meeting**: Fridays 3PM UTC on Discord
- **Monthly Community Call**: First Saturday of each month
- **Quarterly Roadmap Review**: Open to all contributors

### Recognition

We recognize contributors in multiple ways:

- **Contributors file**: All contributors are listed in CONTRIBUTORS.md
- **Release notes**: Significant contributions are highlighted in release notes
- **Social media**: We share and celebrate contributions on our social channels
- **Conference talks**: Opportunities to present work at conferences and meetups

---

## Getting Started Checklist

**Before your first contribution:**

- [ ] Read and understand the Code of Conduct
- [ ] Set up development environment
- [ ] Run tests to ensure everything works
- [ ] Join our Discord community
- [ ] Look through good first issues
- [ ] Introduce yourself in the #newcomers channel

**For your first PR:**

- [ ] Start with a small, focused change
- [ ] Ask questions early and often
- [ ] Follow the coding standards
- [ ] Write tests for your changes
- [ ] Update documentation as needed

---

## FAQ

**Q: How do I know if my feature idea will be accepted?**
A: Create an issue first to discuss the feature. We're happy to provide feedback on whether it fits our roadmap.

**Q: Can I work on multiple issues simultaneously?**
A: We recommend focusing on one issue at a time, especially when starting out.

**Q: What if I can't finish what I started?**
A: That's okay! Just comment on the issue to let others know they can take over.

**Q: How long does the review process take?**
A: We aim to provide initial feedback within 48 hours for small PRs, and within a week for larger changes.

**Q: Can I contribute if I'm new to C++ or neural networks?**
A: Absolutely! We have documentation, tutorial, and beginner-friendly issues. Everyone starts somewhere.

---

Thank you for contributing to Neural Network Visualizer! Your contributions help make neural networks more accessible and understandable for everyone. 🎉