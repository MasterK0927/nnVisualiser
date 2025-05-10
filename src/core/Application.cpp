/**
 * @file Application.cpp
 * @brief Implementation of the main Application class
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include "core/Application.hpp"
#include "core/NeuralNetwork.hpp"
#include "utils/Logger.hpp"
#include <SFML/Graphics.hpp>

namespace nnv {
namespace core {

Application::Application(const utils::ConfigManager& config)
    : config_(config)
    , running_(false)
    , initialized_(false)
    , deltaTime_(0.0f)
    , targetFrameTime_(1.0f / 60.0f) // 60 FPS default
{
    // Set target frame time from config
    int targetFPS = config_.getTargetFPS();
    if (targetFPS > 0) {
        targetFrameTime_ = 1.0f / static_cast<float>(targetFPS);
    }
}

Application::~Application() {
    shutdown();
}

bool Application::initialize() {
    if (initialized_) {
        NNV_LOG_WARNING("Application already initialized");
        return true;
    }
    
    NNV_LOG_INFO("Initializing Neural Network Visualizer application...");
    
    // Initialize window
    if (!initializeWindow()) {
        NNV_LOG_ERROR("Failed to initialize window");
        return false;
    }
    
    // Initialize subsystems
    if (!initializeSubsystems()) {
        NNV_LOG_ERROR("Failed to initialize subsystems");
        return false;
    }
    
    // Load network if specified in config
    std::string networkFile = config_.get<std::string>("startup.network_file", "");
    if (!networkFile.empty()) {
        if (!loadNeuralNetwork(networkFile)) {
            NNV_LOG_WARNING("Failed to load startup network: {}", networkFile);
        }
    }
    
    initialized_ = true;
    NNV_LOG_INFO("Application initialized successfully");
    return true;
}

int Application::run() {
    if (!initialized_) {
        NNV_LOG_ERROR("Application not initialized");
        return 1;
    }
    
    running_ = true;
    deltaClock_.restart();
    
    NNV_LOG_INFO("Starting main application loop");
    
    while (running_ && window_->isOpen()) {
        // Calculate delta time
        deltaTime_ = deltaClock_.restart().asSeconds();
        
        // Process events
        processEvents();
        
        // Update application logic
        update(deltaTime_);
        
        // Render
        render();
        
        // Limit frame rate
        limitFrameRate();
        
        // Update performance stats
        updatePerformanceStats();
    }
    
    NNV_LOG_INFO("Main application loop ended");
    return 0;
}

void Application::shutdown() {
    if (!initialized_) {
        return;
    }
    
    NNV_LOG_INFO("Shutting down application...");
    
    running_ = false;
    
    // Stop any running neural network training
    if (neuralNetwork_ && neuralNetwork_->isTraining()) {
        neuralNetwork_->stopTraining();
    }
    
    // Cleanup subsystems
    performanceMonitor_.reset();
    inputHandler_.reset();
    uiManager_.reset();
    visualizer_.reset();
    
    // Cleanup window
    if (window_) {
        window_->close();
        window_.reset();
    }
    
    initialized_ = false;
    NNV_LOG_INFO("Application shutdown complete");
}

bool Application::createNeuralNetwork(const NetworkConfig& config) {
    try {
        neuralNetwork_ = std::make_shared<DefaultNetwork>(config);
        NNV_LOG_INFO("Created neural network: {}", config.name);
        return true;
    } catch (const std::exception& e) {
        NNV_LOG_ERROR("Failed to create neural network: {}", e.what());
        return false;
    }
}

bool Application::loadNeuralNetwork(const std::string& filename) {
    try {
        auto network = std::make_shared<DefaultNetwork>();
        if (network->loadFromFile(filename)) {
            neuralNetwork_ = network;
            NNV_LOG_INFO("Loaded neural network from: {}", filename);
            return true;
        }
    } catch (const std::exception& e) {
        NNV_LOG_ERROR("Failed to load neural network from {}: {}", filename, e.what());
    }
    return false;
}

bool Application::saveNeuralNetwork(const std::string& filename) const {
    if (!neuralNetwork_) {
        NNV_LOG_ERROR("No neural network to save");
        return false;
    }
    
    try {
        if (neuralNetwork_->saveToFile(filename)) {
            NNV_LOG_INFO("Saved neural network to: {}", filename);
            return true;
        }
    } catch (const std::exception& e) {
        NNV_LOG_ERROR("Failed to save neural network to {}: {}", filename, e.what());
    }
    return false;
}

void Application::setNeuralNetwork(std::shared_ptr<DefaultNetwork> network) {
    neuralNetwork_ = network;
    if (network) {
        NNV_LOG_INFO("Set neural network: {}", network->getName());
    } else {
        NNV_LOG_INFO("Cleared neural network");
    }
}

bool Application::initializeWindow() {
    int width = config_.getWindowWidth();
    int height = config_.getWindowHeight();
    bool fullscreen = config_.getWindowFullscreen();
    bool vsync = config_.getWindowVSync();
    
    sf::Uint32 style = sf::Style::Default;
    if (fullscreen) {
        style = sf::Style::Fullscreen;
    }
    
    window_ = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(width, height), 
        "Neural Network Visualizer", 
        style
    );
    
    if (!window_) {
        NNV_LOG_ERROR("Failed to create SFML window");
        return false;
    }
    
    window_->setVerticalSyncEnabled(vsync);
    
    NNV_LOG_INFO("Created window: {}x{} (fullscreen: {}, vsync: {})", 
                width, height, fullscreen, vsync);
    return true;
}

bool Application::initializeSubsystems() {
    // For now, create placeholder subsystems
    // These will be implemented in subsequent tasks
    
    NNV_LOG_DEBUG("Initializing subsystems...");
    
    // Performance monitor would be initialized here
    // performanceMonitor_ = std::make_unique<utils::PerformanceMonitor>();
    
    // UI manager would be initialized here
    // uiManager_ = std::make_unique<ui::UIManager>(*window_);
    
    // Input handler would be initialized here
    // inputHandler_ = std::make_unique<ui::InputHandler>(*window_);
    
    // Visualizer would be initialized here
    // visualizer_ = std::make_unique<graphics::Visualizer>(*window_);
    
    NNV_LOG_DEBUG("Subsystems initialized (placeholders)");
    return true;
}

void Application::processEvents() {
    sf::Event event;
    while (window_->pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                requestExit();
                break;
                
            case sf::Event::Resized:
                handleResize(event.size.width, event.size.height);
                break;
                
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape) {
                    requestExit();
                }
                break;
                
            default:
                break;
        }
        
        // Forward events to input handler when implemented
        // if (inputHandler_) {
        //     inputHandler_->handleEvent(event);
        // }
    }
}

void Application::update(float deltaTime) {
    // Update subsystems
    // if (performanceMonitor_) {
    //     performanceMonitor_->update(deltaTime);
    // }
    
    // if (uiManager_) {
    //     uiManager_->update(deltaTime);
    // }
    
    // if (visualizer_ && neuralNetwork_) {
    //     visualizer_->update(deltaTime, *neuralNetwork_);
    // }
}

void Application::render() {
    window_->clear(sf::Color::Black);
    
    // Render subsystems
    // if (visualizer_ && neuralNetwork_) {
    //     visualizer_->render(*window_, *neuralNetwork_);
    // }
    
    // if (uiManager_) {
    //     uiManager_->render(*window_);
    // }
    
    // For now, just display a simple message
    // This will be replaced with actual rendering
    
    window_->display();
}

void Application::handleResize(unsigned int width, unsigned int height) {
    // Update window view
    sf::FloatRect visibleArea(0, 0, static_cast<float>(width), static_cast<float>(height));
    window_->setView(sf::View(visibleArea));
    
    // Notify subsystems of resize
    // if (visualizer_) {
    //     visualizer_->handleResize(width, height);
    // }
    
    // if (uiManager_) {
    //     uiManager_->handleResize(width, height);
    // }
    
    NNV_LOG_DEBUG("Window resized to: {}x{}", width, height);
}

void Application::updatePerformanceStats() {
    // This will be implemented when PerformanceMonitor is created
    static int frameCount = 0;
    static sf::Clock fpsClock;
    
    frameCount++;
    if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
        float fps = static_cast<float>(frameCount) / fpsClock.getElapsedTime().asSeconds();
        // NNV_LOG_DEBUG("FPS: {:.1f}", fps);
        
        frameCount = 0;
        fpsClock.restart();
    }
}

void Application::limitFrameRate() {
    static sf::Clock frameClock;
    float frameTime = frameClock.getElapsedTime().asSeconds();
    
    if (frameTime < targetFrameTime_) {
        float sleepTime = targetFrameTime_ - frameTime;
        sf::sleep(sf::seconds(sleepTime));
    }
    
    frameClock.restart();
}

} // namespace core
} // namespace nnv
