/**
 * @file test_neuron.cpp
 * @brief Unit tests for the Neuron class
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include <gtest/gtest.h>
#include "core/Neuron.hpp"
#include "core/ActivationFunctions.hpp"

using namespace nnv::core;

class NeuronTest : public ::testing::Test {
protected:
    void SetUp() override {
        neuron = std::make_unique<Neuron<float>>(42);
    }
    
    void TearDown() override {
        neuron.reset();
    }
    
    std::unique_ptr<Neuron<float>> neuron;
};

TEST_F(NeuronTest, ConstructorSetsId) {
    EXPECT_EQ(neuron->getId(), 42);
}

TEST_F(NeuronTest, DefaultValues) {
    EXPECT_FLOAT_EQ(neuron->getActivation(), 0.0f);
    EXPECT_FLOAT_EQ(neuron->getBias(), 0.0f);
    EXPECT_FLOAT_EQ(neuron->getWeightedInput(), 0.0f);
    EXPECT_FLOAT_EQ(neuron->getGradient(), 0.0f);
    EXPECT_FLOAT_EQ(neuron->getDelta(), 0.0f);
    EXPECT_TRUE(neuron->isTrainable());
    EXPECT_EQ(neuron->getName(), "");
    EXPECT_EQ(neuron->getInputCount(), 0);
}

TEST_F(NeuronTest, SettersAndGetters) {
    neuron->setActivation(0.5f);
    EXPECT_FLOAT_EQ(neuron->getActivation(), 0.5f);
    
    neuron->setBias(0.1f);
    EXPECT_FLOAT_EQ(neuron->getBias(), 0.1f);
    
    neuron->setWeightedInput(1.2f);
    EXPECT_FLOAT_EQ(neuron->getWeightedInput(), 1.2f);
    
    neuron->setGradient(0.3f);
    EXPECT_FLOAT_EQ(neuron->getGradient(), 0.3f);
    
    neuron->setDelta(0.4f);
    EXPECT_FLOAT_EQ(neuron->getDelta(), 0.4f);
    
    neuron->setTrainable(false);
    EXPECT_FALSE(neuron->isTrainable());
    
    neuron->setName("test_neuron");
    EXPECT_EQ(neuron->getName(), "test_neuron");
}

TEST_F(NeuronTest, InputWeights) {
    std::vector<float> weights = {0.1f, 0.2f, 0.3f};
    neuron->setInputWeights(weights);
    
    EXPECT_EQ(neuron->getInputCount(), 3);
    
    auto retrievedWeights = neuron->getInputWeights();
    EXPECT_EQ(retrievedWeights.size(), 3);
    EXPECT_FLOAT_EQ(retrievedWeights[0], 0.1f);
    EXPECT_FLOAT_EQ(retrievedWeights[1], 0.2f);
    EXPECT_FLOAT_EQ(retrievedWeights[2], 0.3f);
    
    neuron->setInputWeight(1, 0.5f);
    EXPECT_FLOAT_EQ(neuron->getInputWeight(1), 0.5f);
}

TEST_F(NeuronTest, ApplyActivation) {
    neuron->setWeightedInput(1.0f);
    neuron->setBias(0.5f);
    
    // Test ReLU activation
    auto reluFunc = activation::relu<float>;
    neuron->applyActivation(reluFunc);
    EXPECT_FLOAT_EQ(neuron->getActivation(), 1.5f); // ReLU(1.0 + 0.5) = 1.5
    
    // Test with negative input
    neuron->setWeightedInput(-2.0f);
    neuron->applyActivation(reluFunc);
    EXPECT_FLOAT_EQ(neuron->getActivation(), 0.0f); // ReLU(-2.0 + 0.5) = ReLU(-1.5) = 0
}

TEST_F(NeuronTest, ComputeActivationDerivative) {
    neuron->setWeightedInput(1.0f);
    neuron->setBias(0.5f);
    
    auto reluDerivFunc = activation::reluDerivative<float>;
    float derivative = neuron->computeActivationDerivative(reluDerivFunc);
    EXPECT_FLOAT_EQ(derivative, 1.0f); // ReLU derivative for positive input
    
    neuron->setWeightedInput(-2.0f);
    derivative = neuron->computeActivationDerivative(reluDerivFunc);
    EXPECT_FLOAT_EQ(derivative, 0.0f); // ReLU derivative for negative input
}

TEST_F(NeuronTest, Reset) {
    neuron->setActivation(0.5f);
    neuron->setWeightedInput(1.0f);
    neuron->setGradient(0.3f);
    neuron->setDelta(0.4f);
    
    neuron->reset();
    
    EXPECT_FLOAT_EQ(neuron->getActivation(), 0.0f);
    EXPECT_FLOAT_EQ(neuron->getWeightedInput(), 0.0f);
    EXPECT_FLOAT_EQ(neuron->getGradient(), 0.0f);
    EXPECT_FLOAT_EQ(neuron->getDelta(), 0.0f);
}

TEST_F(NeuronTest, JsonSerialization) {
    neuron->setActivation(0.5f);
    neuron->setBias(0.1f);
    neuron->setName("test_neuron");
    neuron->setInputWeights({0.1f, 0.2f, 0.3f});
    
    auto json = neuron->toJson();
    
    auto newNeuron = std::make_unique<Neuron<float>>();
    newNeuron->fromJson(json);
    
    EXPECT_FLOAT_EQ(newNeuron->getActivation(), 0.5f);
    EXPECT_FLOAT_EQ(newNeuron->getBias(), 0.1f);
    EXPECT_EQ(newNeuron->getName(), "test_neuron");
    
    auto weights = newNeuron->getInputWeights();
    EXPECT_EQ(weights.size(), 3);
    EXPECT_FLOAT_EQ(weights[0], 0.1f);
    EXPECT_FLOAT_EQ(weights[1], 0.2f);
    EXPECT_FLOAT_EQ(weights[2], 0.3f);
}
