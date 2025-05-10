/**
 * @file test_main.cpp
 * @brief Main test runner for Neural Network Visualizer
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include <gtest/gtest.h>
#include "utils/Logger.hpp"

int main(int argc, char** argv) {
    // Initialize logging for tests
    nnv::utils::Logger::initialize("", nnv::utils::LogLevel::Warning);
    
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run tests
    int result = RUN_ALL_TESTS();
    
    // Cleanup
    nnv::utils::Logger::shutdown();
    
    return result;
}
