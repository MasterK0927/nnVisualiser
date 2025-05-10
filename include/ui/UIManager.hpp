/**
 * @file UIManager.hpp
 * @brief Main UI management system using Dear ImGui
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>

#ifdef HAS_IMGUI
#include <imgui.h>
#include <imgui-SFML.h>
#endif

#include "core/Types.hpp"
#include "graphics/RenderConfig.hpp"
#include "utils/Common.hpp"

// Forward declarations
namespace nnv {
namespace core {
    template<typename T> class NeuralNetwork;
}
namespace ui {
    class NetworkPanel;
    class ConfigPanel;
    class TrainingPanel;
    class VisualizationPanel;
    class PerformancePanel;
    class MenuBar;
}
}

namespace nnv {
namespace ui {

/**
 * @brief UI theme types
 */
enum class UITheme {
    Dark,
    Light,
    Classic,
    Custom
};

/**
 * @brief UI panel types
 */
enum class PanelType {
    Network,
    Configuration,
    Training,
    Visualization,
    Performance,
    Console
};

/**
 * @brief UI event types
 */
enum class UIEventType {
    NetworkChanged,
    ConfigChanged,
    TrainingStarted,
    TrainingStopped,
    FileLoaded,
    FileSaved
};

/**
 * @brief UI event data
 */
struct UIEvent {
    UIEventType type;
    std::string data;
    void* userData = nullptr;
};

/**
 * @brief UI event callback
 */
using UIEventCallback = std::function<void(const UIEvent&)>;

/**
 * @brief Main UI manager class
 */
class UIManager {
public:
    /**
     * @brief Constructor
     * @param window SFML render window
     */
    explicit UIManager(sf::RenderWindow& window);
    
    /**
     * @brief Destructor
     */
    ~UIManager();
    
    // Disable copy and move
    NNV_DISABLE_COPY_AND_MOVE(UIManager)
    
    /**
     * @brief Initialize UI system
     * @return True if successful
     */
    bool initialize();
    
    /**
     * @brief Shutdown UI system
     */
    void shutdown();
    
    /**
     * @brief Update UI
     * @param deltaTime Time since last update
     */
    void update(float deltaTime);
    
    /**
     * @brief Handle SFML event
     * @param event SFML event
     * @return True if event was consumed by UI
     */
    bool handleEvent(const sf::Event& event);
    
    /**
     * @brief Render UI
     * @param window Render target
     */
    void render(sf::RenderWindow& window);
    
    /**
     * @brief Handle window resize
     * @param width New window width
     * @param height New window height
     */
    void handleResize(unsigned int width, unsigned int height);
    
    /**
     * @brief Set neural network
     * @param network Neural network to manage
     */
    void setNeuralNetwork(std::shared_ptr<core::DefaultNetwork> network);
    
    /**
     * @brief Get neural network
     * @return Current neural network
     */
    std::shared_ptr<core::DefaultNetwork> getNeuralNetwork() const { return neuralNetwork_; }
    
    /**
     * @brief Set render configuration
     * @param config Render configuration
     */
    void setRenderConfig(graphics::RenderConfig* config);
    
    /**
     * @brief Get render configuration
     * @return Current render configuration
     */
    graphics::RenderConfig* getRenderConfig() const { return renderConfig_; }
    
    /**
     * @brief Set UI theme
     * @param theme UI theme
     */
    void setTheme(UITheme theme);
    
    /**
     * @brief Get UI theme
     * @return Current UI theme
     */
    UITheme getTheme() const { return currentTheme_; }
    
    /**
     * @brief Show/hide panel
     * @param panel Panel type
     * @param visible Visibility state
     */
    void setPanelVisible(PanelType panel, bool visible);
    
    /**
     * @brief Check if panel is visible
     * @param panel Panel type
     * @return True if visible
     */
    bool isPanelVisible(PanelType panel) const;
    
    /**
     * @brief Register event callback
     * @param callback Event callback function
     */
    void registerEventCallback(UIEventCallback callback);
    
    /**
     * @brief Emit UI event
     * @param event Event to emit
     */
    void emitEvent(const UIEvent& event);
    
    /**
     * @brief Show file dialog
     * @param title Dialog title
     * @param filters File filters
     * @param save True for save dialog, false for open
     * @return Selected file path (empty if cancelled)
     */
    std::string showFileDialog(const std::string& title,
                              const std::vector<std::string>& filters,
                              bool save = false);
    
    /**
     * @brief Show message box
     * @param title Message box title
     * @param message Message text
     * @param type Message type (info, warning, error)
     */
    void showMessageBox(const std::string& title,
                       const std::string& message,
                       const std::string& type = "info");
    
    /**
     * @brief Get UI statistics
     * @return UI performance statistics
     */
    struct UIStats {
        float frameTime = 0.0f;
        int drawCalls = 0;
        int activeWindows = 0;
        std::size_t memoryUsage = 0;
    };
    
    UIStats getUIStats() const { return uiStats_; }
    
    /**
     * @brief Enable/disable docking
     * @param enabled Docking state
     */
    void setDockingEnabled(bool enabled);
    
    /**
     * @brief Check if docking is enabled
     * @return True if docking is enabled
     */
    bool isDockingEnabled() const { return dockingEnabled_; }
    
    /**
     * @brief Reset UI layout to default
     */
    void resetLayout();
    
    /**
     * @brief Save UI layout to file
     * @param filename Layout file path
     * @return True if successful
     */
    bool saveLayout(const std::string& filename);
    
    /**
     * @brief Load UI layout from file
     * @param filename Layout file path
     * @return True if successful
     */
    bool loadLayout(const std::string& filename);

private:
    sf::RenderWindow& window_;                          ///< Reference to render window
    std::shared_ptr<core::DefaultNetwork> neuralNetwork_; ///< Current neural network
    graphics::RenderConfig* renderConfig_;              ///< Render configuration
    
    // UI state
    bool initialized_;                                  ///< Initialization flag
    UITheme currentTheme_;                              ///< Current UI theme
    bool dockingEnabled_;                               ///< Docking enabled flag
    UIStats uiStats_;                                   ///< UI statistics
    
    // Panels
    std::unique_ptr<MenuBar> menuBar_;                  ///< Menu bar
    std::unique_ptr<NetworkPanel> networkPanel_;        ///< Network panel
    std::unique_ptr<ConfigPanel> configPanel_;          ///< Configuration panel
    std::unique_ptr<TrainingPanel> trainingPanel_;      ///< Training panel
    std::unique_ptr<VisualizationPanel> visualizationPanel_; ///< Visualization panel
    std::unique_ptr<PerformancePanel> performancePanel_; ///< Performance panel
    
    // Panel visibility
    std::vector<bool> panelVisibility_;                 ///< Panel visibility states
    
    // Event system
    std::vector<UIEventCallback> eventCallbacks_;       ///< Event callbacks
    
    // Timing
    sf::Clock uiClock_;                                 ///< UI timing
    
#ifdef HAS_IMGUI
    /**
     * @brief Initialize ImGui
     * @return True if successful
     */
    bool initializeImGui();
    
    /**
     * @brief Shutdown ImGui
     */
    void shutdownImGui();
    
    /**
     * @brief Apply ImGui theme
     * @param theme Theme to apply
     */
    void applyImGuiTheme(UITheme theme);
    
    /**
     * @brief Setup ImGui docking
     */
    void setupDocking();
    
    /**
     * @brief Render main dockspace
     */
    void renderDockspace();
    
    /**
     * @brief Update UI statistics
     */
    void updateUIStats();
#endif
    
    /**
     * @brief Initialize panels
     * @return True if successful
     */
    bool initializePanels();
    
    /**
     * @brief Render all panels
     */
    void renderPanels();
    
    /**
     * @brief Get panel visibility reference
     * @param panel Panel type
     * @return Reference to visibility flag
     */
    bool& getPanelVisibilityRef(PanelType panel);
};

} // namespace ui
} // namespace nnv
