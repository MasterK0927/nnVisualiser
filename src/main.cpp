/**
 * @file main.cpp
 * @brief Main entry point for Neural Network Visualizer application
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include <iostream>
#include <memory>
#include <exception>

#include "core/Application.hpp"
#include "utils/Logger.hpp"
#include "utils/ConfigManager.hpp"

/**
 * @brief Main application entry point
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return Exit code (0 for success, non-zero for error)
 */
int main(int argc, char* argv[]) {
    try {
        // Initialize logging system
        nnv::utils::Logger::initialize();
        
        // Log application startup
        NNV_LOG_INFO("Neural Network Visualizer v1.0.0 starting...");
        
        // Parse command line arguments
        nnv::utils::ConfigManager config;
        if (!config.parseCommandLine(argc, argv)) {
            NNV_LOG_ERROR("Failed to parse command line arguments");
            return 1;
        }
        
        // Create and run application
        auto app = std::make_unique<nnv::core::Application>(config);
        
        if (!app->initialize()) {
            NNV_LOG_ERROR("Failed to initialize application");
            return 1;
        }
        
        NNV_LOG_INFO("Application initialized successfully");
        
        // Run main application loop
        int exitCode = app->run();
        
        NNV_LOG_INFO("Application exiting with code: {}", exitCode);
        
        return exitCode;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        NNV_LOG_CRITICAL("Fatal error: {}", e.what());
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        NNV_LOG_CRITICAL("Unknown fatal error occurred");
        return 1;
    }
}
