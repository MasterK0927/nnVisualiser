/**
 * @file simple_xor_demo.cpp
 * @brief Simple demonstration of XOR problem solving
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include <iostream>
#include <vector>
#include <iomanip>

#include "core/NeuralNetwork.hpp"
#include "core/Types.hpp"
#include "utils/Logger.hpp"
#include "utils/ConfigManager.hpp"

using namespace nnv;

int main() {
    // Initialize logging
    utils::Logger::initialize("", utils::LogLevel::Info);
    
    std::cout << "Neural Network Visualizer - XOR Problem Demo\n";
    std::cout << "============================================\n\n";
    
    try {
        // Create network configuration for XOR problem
        core::NetworkConfig config;
        config.name = "XOR Solver";
        
        // Input layer (2 neurons)
        core::LayerConfig inputLayer;
        inputLayer.size = 2;
        inputLayer.activation = core::ActivationType::None;
        inputLayer.name = "input";
        config.layers.push_back(inputLayer);
        
        // Hidden layer (4 neurons with ReLU)
        core::LayerConfig hiddenLayer;
        hiddenLayer.size = 4;
        hiddenLayer.activation = core::ActivationType::ReLU;
        hiddenLayer.name = "hidden";
        config.layers.push_back(hiddenLayer);
        
        // Output layer (1 neuron with sigmoid)
        core::LayerConfig outputLayer;
        outputLayer.size = 1;
        outputLayer.activation = core::ActivationType::Sigmoid;
        outputLayer.name = "output";
        config.layers.push_back(outputLayer);
        
        // Training configuration
        config.training.learning_rate = 0.1f;
        config.training.batch_size = 4;
        config.training.epochs = 1000;
        
        // Loss function
        config.loss = core::LossType::MeanSquaredError;
        config.optimizer = core::OptimizerType::SGD;
        
        // Create neural network
        std::cout << "Creating neural network...\n";
        auto network = std::make_unique<core::NeuralNetwork<float>>(config);
        
        // XOR training data
        std::vector<std::vector<float>> inputs = {
            {0.0f, 0.0f},  // XOR(0,0) = 0
            {0.0f, 1.0f},  // XOR(0,1) = 1
            {1.0f, 0.0f},  // XOR(1,0) = 1
            {1.0f, 1.0f}   // XOR(1,1) = 0
        };
        
        std::vector<std::vector<float>> targets = {
            {0.0f},
            {1.0f},
            {1.0f},
            {0.0f}
        };
        
        std::cout << "Training data:\n";
        for (size_t i = 0; i < inputs.size(); ++i) {
            std::cout << "  Input: [" << inputs[i][0] << ", " << inputs[i][1] 
                      << "] -> Target: " << targets[i][0] << "\n";
        }
        std::cout << "\n";
        
        // Test network before training
        std::cout << "Testing network before training:\n";
        for (size_t i = 0; i < inputs.size(); ++i) {
            auto output = network->predict(inputs[i]);
            std::cout << "  Input: [" << inputs[i][0] << ", " << inputs[i][1] 
                      << "] -> Output: " << std::fixed << std::setprecision(4) 
                      << output[0] << " (Target: " << targets[i][0] << ")\n";
        }
        std::cout << "\n";
        
        // Train the network
        std::cout << "Training network for " << config.training.epochs << " epochs...\n";
        
        auto progressCallback = [](size_t epoch, float loss, float accuracy) {
            if (epoch % 100 == 0 || epoch < 10) {
                std::cout << "  Epoch " << std::setw(4) << epoch 
                          << ": Loss = " << std::fixed << std::setprecision(6) << loss
                          << ", Accuracy = " << std::setprecision(4) << accuracy << "\n";
            }
        };
        
        auto history = network->train(inputs, targets, 
                                     config.training.epochs, 
                                     config.training.batch_size,
                                     nullptr, nullptr,
                                     progressCallback);
        
        std::cout << "\nTraining completed!\n\n";
        
        // Test network after training
        std::cout << "Testing network after training:\n";
        float totalError = 0.0f;
        
        for (size_t i = 0; i < inputs.size(); ++i) {
            auto output = network->predict(inputs[i]);
            float error = std::abs(output[0] - targets[i][0]);
            totalError += error;
            
            std::cout << "  Input: [" << inputs[i][0] << ", " << inputs[i][1] 
                      << "] -> Output: " << std::fixed << std::setprecision(4) 
                      << output[0] << " (Target: " << targets[i][0] 
                      << ", Error: " << std::setprecision(6) << error << ")\n";
        }
        
        float avgError = totalError / inputs.size();
        std::cout << "\nAverage error: " << std::fixed << std::setprecision(6) << avgError << "\n";
        
        // Display training statistics
        if (!history.trainLoss.empty()) {
            std::cout << "\nTraining Statistics:\n";
            std::cout << "  Initial loss: " << std::fixed << std::setprecision(6) 
                      << history.trainLoss.front() << "\n";
            std::cout << "  Final loss: " << std::setprecision(6) 
                      << history.trainLoss.back() << "\n";
            std::cout << "  Final accuracy: " << std::setprecision(4) 
                      << history.trainAccuracy.back() << "\n";
        }
        
        // Save the trained network
        std::string modelFile = "xor_trained_model.json";
        if (network->saveToFile(modelFile)) {
            std::cout << "\nTrained model saved to: " << modelFile << "\n";
        }
        
        // Test loading the saved model
        std::cout << "\nTesting model loading...\n";
        auto loadedNetwork = std::make_unique<core::NeuralNetwork<float>>();
        if (loadedNetwork->loadFromFile(modelFile)) {
            std::cout << "Model loaded successfully!\n";
            
            // Test loaded model
            std::cout << "Testing loaded model:\n";
            for (size_t i = 0; i < inputs.size(); ++i) {
                auto output = loadedNetwork->predict(inputs[i]);
                std::cout << "  Input: [" << inputs[i][0] << ", " << inputs[i][1] 
                          << "] -> Output: " << std::fixed << std::setprecision(4) 
                          << output[0] << "\n";
            }
        } else {
            std::cout << "Failed to load model!\n";
        }
        
        std::cout << "\nDemo completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    // Cleanup
    utils::Logger::shutdown();
    return 0;
}
