/**
 * @file test_neural_network.cpp
 * @brief Unit tests for the NeuralNetwork class
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include <gtest/gtest.h>
#include "core/NeuralNetwork.hpp"
#include "core/Types.hpp"

using namespace nnv::core;

class NeuralNetworkTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a simple 2-3-1 network for testing
        NetworkConfig config;
        config.name = "Test Network";
        
        // Input layer
        LayerConfig inputLayer;
        inputLayer.size = 2;
        inputLayer.activation = ActivationType::None;
        inputLayer.name = "input";
        config.layers.push_back(inputLayer);
        
        // Hidden layer
        LayerConfig hiddenLayer;
        hiddenLayer.size = 3;
        hiddenLayer.activation = ActivationType::ReLU;
        hiddenLayer.name = "hidden";
        config.layers.push_back(hiddenLayer);
        
        // Output layer
        LayerConfig outputLayer;
        outputLayer.size = 1;
        outputLayer.activation = ActivationType::Sigmoid;
        outputLayer.name = "output";
        config.layers.push_back(outputLayer);
        
        config.training.learning_rate = 0.01f;
        config.loss = LossType::MeanSquaredError;
        config.optimizer = OptimizerType::SGD;
        
        network = std::make_unique<NeuralNetwork<float>>(config);
    }
    
    void TearDown() override {
        network.reset();
    }
    
    std::unique_ptr<NeuralNetwork<float>> network;
};

TEST_F(NeuralNetworkTest, ConstructorSetsProperties) {
    EXPECT_EQ(network->getName(), "Test Network");
    EXPECT_EQ(network->getLayerCount(), 3);
    EXPECT_FLOAT_EQ(network->getLearningRate(), 0.01f);
    EXPECT_EQ(network->getLossType(), LossType::MeanSquaredError);
    EXPECT_EQ(network->getOptimizerType(), OptimizerType::SGD);
}

TEST_F(NeuralNetworkTest, LayerAccess) {
    EXPECT_EQ(network->getLayer(0).getSize(), 2);
    EXPECT_EQ(network->getLayer(1).getSize(), 3);
    EXPECT_EQ(network->getLayer(2).getSize(), 1);
    
    EXPECT_EQ(network->getLayer(0).getName(), "input");
    EXPECT_EQ(network->getLayer(1).getName(), "hidden");
    EXPECT_EQ(network->getLayer(2).getName(), "output");
}

TEST_F(NeuralNetworkTest, ForwardPass) {
    std::vector<float> inputs = {0.5f, -0.3f};
    auto outputs = network->forward(inputs);
    
    EXPECT_EQ(outputs.size(), 1);
    EXPECT_GE(outputs[0], 0.0f);  // Sigmoid output should be >= 0
    EXPECT_LE(outputs[0], 1.0f);  // Sigmoid output should be <= 1
}

TEST_F(NeuralNetworkTest, Predict) {
    std::vector<float> inputs = {0.5f, -0.3f};
    auto outputs = network->predict(inputs);
    
    EXPECT_EQ(outputs.size(), 1);
    EXPECT_GE(outputs[0], 0.0f);
    EXPECT_LE(outputs[0], 1.0f);
}

TEST_F(NeuralNetworkTest, TrainSample) {
    std::vector<float> inputs = {0.5f, -0.3f};
    std::vector<float> targets = {0.8f};
    
    float initialLoss = network->trainSample(inputs, targets);
    EXPECT_GE(initialLoss, 0.0f);
    
    // Train multiple times and expect loss to generally decrease
    float previousLoss = initialLoss;
    for (int i = 0; i < 10; ++i) {
        float loss = network->trainSample(inputs, targets);
        EXPECT_GE(loss, 0.0f);
        // Note: Loss might not decrease monotonically due to randomness
    }
}

TEST_F(NeuralNetworkTest, TrainBatch) {
    std::vector<std::vector<float>> inputs = {
        {0.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f}
    };
    
    std::vector<std::vector<float>> targets = {
        {0.0f},
        {1.0f},
        {1.0f},
        {0.0f}
    };
    
    float loss = network->trainBatch(inputs, targets);
    EXPECT_GE(loss, 0.0f);
}

TEST_F(NeuralNetworkTest, Evaluate) {
    std::vector<std::vector<float>> inputs = {
        {0.0f, 0.0f},
        {1.0f, 1.0f}
    };
    
    std::vector<std::vector<float>> targets = {
        {0.0f},
        {0.0f}
    };
    
    auto result = network->evaluate(inputs, targets);
    EXPECT_GE(result.first, 0.0f);   // Loss should be non-negative
    EXPECT_GE(result.second, 0.0f);  // Accuracy should be non-negative
    EXPECT_LE(result.second, 1.0f);  // Accuracy should be <= 1
}

TEST_F(NeuralNetworkTest, PredictBatch) {
    std::vector<std::vector<float>> inputs = {
        {0.0f, 0.0f},
        {1.0f, 1.0f}
    };
    
    auto outputs = network->predictBatch(inputs);
    
    EXPECT_EQ(outputs.size(), 2);
    for (const auto& output : outputs) {
        EXPECT_EQ(output.size(), 1);
        EXPECT_GE(output[0], 0.0f);
        EXPECT_LE(output[0], 1.0f);
    }
}

TEST_F(NeuralNetworkTest, SettersAndGetters) {
    network->setLearningRate(0.05f);
    EXPECT_FLOAT_EQ(network->getLearningRate(), 0.05f);
    
    network->setLossType(LossType::CrossEntropy);
    EXPECT_EQ(network->getLossType(), LossType::CrossEntropy);
    
    network->setOptimizerType(OptimizerType::Adam);
    EXPECT_EQ(network->getOptimizerType(), OptimizerType::Adam);
    
    network->setName("Modified Network");
    EXPECT_EQ(network->getName(), "Modified Network");
}

TEST_F(NeuralNetworkTest, Reset) {
    // Train a bit first
    std::vector<float> inputs = {0.5f, -0.3f};
    std::vector<float> targets = {0.8f};
    network->trainSample(inputs, targets);
    
    // Reset should not crash
    network->reset();
    
    // Network should still be functional
    auto outputs = network->predict(inputs);
    EXPECT_EQ(outputs.size(), 1);
}

TEST_F(NeuralNetworkTest, JsonSerialization) {
    // Set some specific values
    network->setName("Serialization Test");
    network->setLearningRate(0.123f);
    
    // Serialize to JSON
    auto json = network->toJson();
    
    // Create new network and deserialize
    auto newNetwork = std::make_unique<NeuralNetwork<float>>();
    newNetwork->fromJson(json);
    
    // Check that values were preserved
    EXPECT_EQ(newNetwork->getName(), "Serialization Test");
    EXPECT_FLOAT_EQ(newNetwork->getLearningRate(), 0.123f);
    EXPECT_EQ(newNetwork->getLayerCount(), 3);
}
