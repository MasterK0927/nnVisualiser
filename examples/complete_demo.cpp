/**
 * @file complete_demo.cpp
 * @brief Complete demonstration of the Neural Network Visualizer
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>

// Core includes
#include "core/NeuralNetwork.hpp"
#include "core/Application.hpp"
#include "utils/Logger.hpp"
#include "utils/ConfigManager.hpp"
#include "utils/DataLoader.hpp"

// Graphics includes
#include "graphics/Visualizer.hpp"
#include "graphics/RenderConfig.hpp"
#include "graphics/ColorScheme.hpp"

// UI includes (if available)
#ifdef HAS_IMGUI
#include "ui/UIManager.hpp"
#include "ui/NetworkPanel.hpp"
#endif

using namespace nnv;

/**
 * @brief Complete demo application class
 */
class CompleteDemoApp {
public:
    CompleteDemoApp() 
        : window_(sf::VideoMode(1200, 800), "Neural Network Visualizer - Complete Demo")
        , running_(false)
    {
        window_.setVerticalSyncEnabled(true);
    }
    
    bool initialize() {
        // Initialize logging
        utils::Logger::initialize("demo.log", utils::LogLevel::Info);
        NNV_LOG_INFO("Starting Neural Network Visualizer Complete Demo");
        
        // Load configuration
        config_ = std::make_unique<utils::ConfigManager>();
        if (!config_->loadFromFile("demo_config.json")) {
            NNV_LOG_WARNING("Could not load config file, using defaults");
        }
        
        // Create neural network for XOR problem
        if (!createXORNetwork()) {
            NNV_LOG_ERROR("Failed to create XOR network");
            return false;
        }
        
        // Initialize graphics system
        if (!initializeGraphics()) {
            NNV_LOG_ERROR("Failed to initialize graphics");
            return false;
        }
        
        // Initialize UI system
        if (!initializeUI()) {
            NNV_LOG_ERROR("Failed to initialize UI");
            return false;
        }
        
        // Load and prepare training data
        if (!prepareTrainingData()) {
            NNV_LOG_ERROR("Failed to prepare training data");
            return false;
        }
        
        NNV_LOG_INFO("Demo application initialized successfully");
        return true;
    }
    
    int run() {
        if (!initialize()) {
            return 1;
        }
        
        running_ = true;
        sf::Clock deltaClock;
        
        NNV_LOG_INFO("Starting main demo loop");
        
        while (running_ && window_.isOpen()) {
            float deltaTime = deltaClock.restart().asSeconds();
            
            processEvents();
            update(deltaTime);
            render();
        }
        
        shutdown();
        return 0;
    }

private:
    sf::RenderWindow window_;
    bool running_;
    
    // Core systems
    std::unique_ptr<utils::ConfigManager> config_;
    std::shared_ptr<core::NeuralNetwork<float>> network_;
    std::unique_ptr<graphics::Visualizer> visualizer_;
    
#ifdef HAS_IMGUI
    std::unique_ptr<ui::UIManager> uiManager_;
#endif
    
    // Training data
    std::vector<std::vector<float>> trainingInputs_;
    std::vector<std::vector<float>> trainingTargets_;
    
    // Demo state
    bool isTraining_;
    bool showTrainingProgress_;
    int currentEpoch_;
    float currentLoss_;
    
    bool createXORNetwork() {
        try {
            // Create network configuration for XOR problem
            core::NetworkConfig config;
            config.name = "XOR Solver Demo";
            
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
            
            // Loss function and optimizer
            config.loss = core::LossType::MeanSquaredError;
            config.optimizer = core::OptimizerType::SGD;
            
            network_ = std::make_shared<core::NeuralNetwork<float>>(config);
            
            NNV_LOG_INFO("Created XOR network with {} layers", network_->getLayerCount());
            return true;
            
        } catch (const std::exception& e) {
            NNV_LOG_ERROR("Failed to create XOR network: {}", e.what());
            return false;
        }
    }
    
    bool initializeGraphics() {
        try {
            visualizer_ = std::make_unique<graphics::Visualizer>(window_);
            
            if (!visualizer_->initialize()) {
                return false;
            }
            
            // Configure visualization
            auto& renderConfig = visualizer_->getRenderConfig();
            renderConfig.layout = graphics::LayoutType::Horizontal;
            renderConfig.colorScheme = graphics::ColorScheme::BlueGradient;
            renderConfig.neuron.radius = 20.0f;
            renderConfig.neuron.showValue = true;
            renderConfig.connection.thickness = 3.0f;
            renderConfig.connection.animateFlow = true;
            renderConfig.animation.enabled = true;
            renderConfig.animation.duration = 0.5f;
            
            // Apply color scheme
            renderConfig.applyColorScheme(graphics::ColorScheme::BlueGradient);
            
            // Fit network to view
            visualizer_->fitToView(*network_);
            
            NNV_LOG_INFO("Graphics system initialized");
            return true;
            
        } catch (const std::exception& e) {
            NNV_LOG_ERROR("Failed to initialize graphics: {}", e.what());
            return false;
        }
    }
    
    bool initializeUI() {
#ifdef HAS_IMGUI
        try {
            uiManager_ = std::make_unique<ui::UIManager>(window_);
            
            if (!uiManager_->initialize()) {
                return false;
            }
            
            // Set up UI
            uiManager_->setNeuralNetwork(network_);
            uiManager_->setRenderConfig(&visualizer_->getRenderConfig());
            uiManager_->setTheme(ui::UITheme::Dark);
            uiManager_->setDockingEnabled(true);
            
            // Register event callbacks
            uiManager_->registerEventCallback([this](const ui::UIEvent& event) {
                handleUIEvent(event);
            });
            
            NNV_LOG_INFO("UI system initialized");
            return true;
            
        } catch (const std::exception& e) {
            NNV_LOG_ERROR("Failed to initialize UI: {}", e.what());
            return false;
        }
#else
        NNV_LOG_INFO("UI system not available (ImGui not found)");
        return true;
#endif
    }
    
    bool prepareTrainingData() {
        // XOR training data
        trainingInputs_ = {
            {0.0f, 0.0f},  // XOR(0,0) = 0
            {0.0f, 1.0f},  // XOR(0,1) = 1
            {1.0f, 0.0f},  // XOR(1,0) = 1
            {1.0f, 1.0f}   // XOR(1,1) = 0
        };
        
        trainingTargets_ = {
            {0.0f},
            {1.0f},
            {1.0f},
            {0.0f}
        };
        
        isTraining_ = false;
        showTrainingProgress_ = true;
        currentEpoch_ = 0;
        currentLoss_ = 0.0f;
        
        NNV_LOG_INFO("Prepared XOR training data: {} samples", trainingInputs_.size());
        return true;
    }
    
    void processEvents() {
        sf::Event event;
        while (window_.pollEvent(event)) {
            bool eventHandled = false;
            
#ifdef HAS_IMGUI
            // Let UI handle events first
            if (uiManager_) {
                eventHandled = uiManager_->handleEvent(event);
            }
#endif
            
            // Let visualizer handle events if UI didn't consume them
            if (!eventHandled && visualizer_) {
                if (event.type == sf::Event::MouseButtonPressed ||
                    event.type == sf::Event::MouseButtonReleased ||
                    event.type == sf::Event::MouseMoved ||
                    event.type == sf::Event::MouseWheelScrolled) {
                    eventHandled = visualizer_->handleMouseEvent(event);
                } else if (event.type == sf::Event::KeyPressed ||
                          event.type == sf::Event::KeyReleased) {
                    eventHandled = visualizer_->handleKeyboardEvent(event);
                }
            }
            
            // Handle application events
            if (!eventHandled) {
                switch (event.type) {
                    case sf::Event::Closed:
                        running_ = false;
                        break;
                        
                    case sf::Event::Resized:
                        handleResize(event.size.width, event.size.height);
                        break;
                        
                    case sf::Event::KeyPressed:
                        handleKeyPress(event.key.code);
                        break;
                        
                    default:
                        break;
                }
            }
        }
    }
    
    void update(float deltaTime) {
        // Update visualizer
        if (visualizer_ && network_) {
            visualizer_->update(deltaTime, *network_);
        }
        
#ifdef HAS_IMGUI
        // Update UI
        if (uiManager_) {
            uiManager_->update(deltaTime);
        }
#endif
        
        // Update training if active
        if (isTraining_ && network_) {
            updateTraining();
        }
    }
    
    void render() {
        window_.clear(sf::Color::Black);
        
        // Render visualization
        if (visualizer_ && network_) {
            visualizer_->render(window_, *network_);
        }
        
#ifdef HAS_IMGUI
        // Render UI
        if (uiManager_) {
            uiManager_->render(window_);
        }
#endif
        
        window_.display();
    }
    
    void handleResize(unsigned int width, unsigned int height) {
        sf::FloatRect visibleArea(0, 0, static_cast<float>(width), static_cast<float>(height));
        window_.setView(sf::View(visibleArea));
        
        if (visualizer_) {
            visualizer_->handleResize(width, height);
        }
        
#ifdef HAS_IMGUI
        if (uiManager_) {
            uiManager_->handleResize(width, height);
        }
#endif
    }
    
    void handleKeyPress(sf::Keyboard::Key key) {
        switch (key) {
            case sf::Keyboard::Escape:
                running_ = false;
                break;
                
            case sf::Keyboard::Space:
                toggleTraining();
                break;
                
            case sf::Keyboard::R:
                resetNetwork();
                break;
                
            case sf::Keyboard::T:
                testNetwork();
                break;
                
            case sf::Keyboard::S:
                saveNetwork();
                break;
                
            default:
                break;
        }
    }
    
#ifdef HAS_IMGUI
    void handleUIEvent(const ui::UIEvent& event) {
        switch (event.type) {
            case ui::UIEventType::NetworkChanged:
                NNV_LOG_INFO("Network changed via UI");
                break;
                
            case ui::UIEventType::TrainingStarted:
                startTraining();
                break;
                
            case ui::UIEventType::TrainingStopped:
                stopTraining();
                break;
                
            default:
                break;
        }
    }
#endif
    
    void toggleTraining() {
        if (isTraining_) {
            stopTraining();
        } else {
            startTraining();
        }
    }
    
    void startTraining() {
        if (!network_ || isTraining_) {
            return;
        }
        
        isTraining_ = true;
        currentEpoch_ = 0;
        
        NNV_LOG_INFO("Started training XOR network");
        
        // Start training in a separate thread (simplified for demo)
        // In a real application, you'd want proper thread management
    }
    
    void stopTraining() {
        if (!isTraining_) {
            return;
        }
        
        isTraining_ = false;
        if (network_) {
            network_->stopTraining();
        }
        
        NNV_LOG_INFO("Stopped training at epoch {}", currentEpoch_);
    }
    
    void updateTraining() {
        if (!network_ || !isTraining_) {
            return;
        }
        
        // Perform a few training steps per frame
        for (int i = 0; i < 5 && isTraining_; ++i) {
            currentLoss_ = network_->trainBatch(trainingInputs_, trainingTargets_);
            currentEpoch_++;
            
            // Stop training after 1000 epochs or if loss is very low
            if (currentEpoch_ >= 1000 || currentLoss_ < 0.001f) {
                stopTraining();
                testNetwork();
                break;
            }
        }
    }
    
    void resetNetwork() {
        if (network_) {
            network_->reset();
            network_->initializeWeights();
            currentEpoch_ = 0;
            currentLoss_ = 0.0f;
            NNV_LOG_INFO("Reset network weights");
        }
    }
    
    void testNetwork() {
        if (!network_) {
            return;
        }
        
        NNV_LOG_INFO("Testing XOR network:");
        
        for (std::size_t i = 0; i < trainingInputs_.size(); ++i) {
            auto output = network_->predict(trainingInputs_[i]);
            float error = std::abs(output[0] - trainingTargets_[i][0]);
            
            NNV_LOG_INFO("  Input: [{:.1f}, {:.1f}] -> Output: {:.4f} (Target: {:.1f}, Error: {:.4f})",
                        trainingInputs_[i][0], trainingInputs_[i][1],
                        output[0], trainingTargets_[i][0], error);
        }
    }
    
    void saveNetwork() {
        if (network_) {
            std::string filename = "demo_network_" + std::to_string(currentEpoch_) + ".json";
            if (network_->saveToFile(filename)) {
                NNV_LOG_INFO("Saved network to: {}", filename);
            }
        }
    }
    
    void shutdown() {
        NNV_LOG_INFO("Shutting down demo application");
        
        if (isTraining_) {
            stopTraining();
        }
        
#ifdef HAS_IMGUI
        if (uiManager_) {
            uiManager_->shutdown();
        }
#endif
        
        visualizer_.reset();
        network_.reset();
        config_.reset();
        
        utils::Logger::shutdown();
    }
};

int main(int argc, char* argv[]) {
    try {
        CompleteDemoApp app;
        return app.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}
