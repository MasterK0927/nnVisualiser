/**
 * @file Application.hpp
 * @brief Main application class for the Neural Network Visualizer
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

#include "core/Types.hpp"
#include "utils/ConfigManager.hpp"
#include "utils/Common.hpp"

// Forward declarations
namespace nnv {
namespace core {
    template<typename T> class NeuralNetwork;
}
namespace graphics {
    class Visualizer;
}
namespace ui {
    class UIManager;
    class InputHandler;
}
namespace utils {
    class PerformanceMonitor;
}
}

namespace nnv {
namespace core {

/**
 * @brief Main application class that orchestrates all components
 */
class Application {
public:
    /**
     * @brief Constructor
     * @param config Configuration manager instance
     */
    explicit Application(const utils::ConfigManager& config);
    
    /**
     * @brief Destructor
     */
    ~Application();
    
    // Disable copy and move
    NNV_DISABLE_COPY_AND_MOVE(Application)
    
    /**
     * @brief Initialize the application
     * @return True if initialization was successful
     */
    bool initialize();
    
    /**
     * @brief Run the main application loop
     * @return Exit code (0 for success)
     */
    int run();
    
    /**
     * @brief Shutdown the application
     */
    void shutdown();
    
    /**
     * @brief Check if the application is running
     * @return True if running
     */
    bool isRunning() const { return running_; }
    
    /**
     * @brief Request application exit
     */
    void requestExit() { running_ = false; }
    
    /**
     * @brief Get the SFML render window
     * @return Reference to the render window
     */
    sf::RenderWindow& getWindow() { return *window_; }
    const sf::RenderWindow& getWindow() const { return *window_; }
    
    /**
     * @brief Get the current neural network
     * @return Pointer to the neural network (may be null)
     */
    std::shared_ptr<DefaultNetwork> getNeuralNetwork() const { return neuralNetwork_; }
    
    /**
     * @brief Set the current neural network
     * @param network New neural network to use
     */
    void setNeuralNetwork(std::shared_ptr<DefaultNetwork> network);
    
    /**
     * @brief Create a new neural network from configuration
     * @param config Network configuration
     * @return True if creation was successful
     */
    bool createNeuralNetwork(const NetworkConfig& config);
    
    /**
     * @brief Load a neural network from file
     * @param filename Path to the network file
     * @return True if loading was successful
     */
    bool loadNeuralNetwork(const std::string& filename);
    
    /**
     * @brief Save the current neural network to file
     * @param filename Path to save the network
     * @return True if saving was successful
     */
    bool saveNeuralNetwork(const std::string& filename) const;
    
    /**
     * @brief Get the configuration manager
     * @return Reference to the configuration manager
     */
    const utils::ConfigManager& getConfig() const { return config_; }
    utils::ConfigManager& getConfig() { return config_; }
    
    /**
     * @brief Get the performance monitor
     * @return Reference to the performance monitor
     */
    const utils::PerformanceMonitor& getPerformanceMonitor() const { return *performanceMonitor_; }
    
private:
    // Configuration
    utils::ConfigManager config_;
    
    // Core components
    std::unique_ptr<sf::RenderWindow> window_;
    std::shared_ptr<DefaultNetwork> neuralNetwork_;
    
    // Subsystems
    std::unique_ptr<graphics::Visualizer> visualizer_;
    std::unique_ptr<ui::UIManager> uiManager_;
    std::unique_ptr<ui::InputHandler> inputHandler_;
    std::unique_ptr<utils::PerformanceMonitor> performanceMonitor_;
    
    // Application state
    bool running_;
    bool initialized_;
    
    // Timing
    sf::Clock deltaClock_;
    float deltaTime_;
    float targetFrameTime_;
    
    /**
     * @brief Initialize the render window
     * @return True if initialization was successful
     */
    bool initializeWindow();
    
    /**
     * @brief Initialize all subsystems
     * @return True if initialization was successful
     */
    bool initializeSubsystems();
    
    /**
     * @brief Process SFML events
     */
    void processEvents();
    
    /**
     * @brief Update application logic
     * @param deltaTime Time since last update in seconds
     */
    void update(float deltaTime);
    
    /**
     * @brief Render the application
     */
    void render();
    
    /**
     * @brief Handle window resize event
     * @param width New window width
     * @param height New window height
     */
    void handleResize(unsigned int width, unsigned int height);
    
    /**
     * @brief Update performance statistics
     */
    void updatePerformanceStats();
    
    /**
     * @brief Limit frame rate if necessary
     */
    void limitFrameRate();
};

} // namespace core
} // namespace nnv
