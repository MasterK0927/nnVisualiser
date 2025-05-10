/**
 * @file Visualizer.hpp
 * @brief Main visualization coordinator for neural networks
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>

#include "core/Types.hpp"
#include "graphics/RenderConfig.hpp"
#include "graphics/ColorScheme.hpp"
#include "utils/Common.hpp"

// Forward declarations
namespace nnv {
namespace core {
    template<typename T> class NeuralNetwork;
}
namespace graphics {
    class NetworkRenderer;
    class LayoutManager;
    class AnimationSystem;
}
}

namespace nnv {
namespace graphics {

/**
 * @brief Main visualization coordinator class
 */
class Visualizer {
public:
    /**
     * @brief Constructor
     * @param window SFML render window
     */
    explicit Visualizer(sf::RenderWindow& window);
    
    /**
     * @brief Destructor
     */
    ~Visualizer();
    
    // Disable copy and move
    NNV_DISABLE_COPY_AND_MOVE(Visualizer)
    
    /**
     * @brief Initialize the visualizer
     * @return True if successful
     */
    bool initialize();
    
    /**
     * @brief Update visualization
     * @param deltaTime Time since last update
     * @param network Neural network to visualize
     */
    void update(float deltaTime, const core::DefaultNetwork& network);
    
    /**
     * @brief Render the visualization
     * @param window Render target
     * @param network Neural network to render
     */
    void render(sf::RenderWindow& window, const core::DefaultNetwork& network);
    
    /**
     * @brief Handle window resize
     * @param width New window width
     * @param height New window height
     */
    void handleResize(unsigned int width, unsigned int height);
    
    /**
     * @brief Handle mouse input
     * @param event Mouse event
     * @return True if event was handled
     */
    bool handleMouseEvent(const sf::Event& event);
    
    /**
     * @brief Handle keyboard input
     * @param event Keyboard event
     * @return True if event was handled
     */
    bool handleKeyboardEvent(const sf::Event& event);
    
    /**
     * @brief Get render configuration
     * @return Reference to render configuration
     */
    RenderConfig& getRenderConfig() { return renderConfig_; }
    const RenderConfig& getRenderConfig() const { return renderConfig_; }
    
    /**
     * @brief Set render configuration
     * @param config New render configuration
     */
    void setRenderConfig(const RenderConfig& config);
    
    /**
     * @brief Get color scheme manager
     * @return Reference to color scheme manager
     */
    ColorSchemeManager& getColorSchemeManager() { return colorSchemeManager_; }
    const ColorSchemeManager& getColorSchemeManager() const { return colorSchemeManager_; }
    
    /**
     * @brief Get render statistics
     * @return Current render statistics
     */
    const RenderStats& getRenderStats() const { return renderStats_; }
    
    /**
     * @brief Get viewport information
     * @return Current viewport
     */
    const Viewport& getViewport() const { return viewport_; }
    
    /**
     * @brief Set zoom level
     * @param zoom New zoom level
     */
    void setZoom(float zoom);
    
    /**
     * @brief Set pan offset
     * @param pan New pan offset
     */
    void setPan(const sf::Vector2f& pan);
    
    /**
     * @brief Reset view to default
     */
    void resetView();
    
    /**
     * @brief Fit network to view
     * @param network Neural network to fit
     */
    void fitToView(const core::DefaultNetwork& network);
    
    /**
     * @brief Get neuron at screen position
     * @param screenPos Screen position
     * @param network Neural network
     * @return Neuron information (layer index, neuron index) or {-1, -1} if none
     */
    std::pair<int, int> getNeuronAtPosition(const sf::Vector2f& screenPos, 
                                           const core::DefaultNetwork& network) const;
    
    /**
     * @brief Highlight neuron
     * @param layerIndex Layer index (-1 to clear)
     * @param neuronIndex Neuron index
     */
    void highlightNeuron(int layerIndex, int neuronIndex);
    
    /**
     * @brief Clear all highlights
     */
    void clearHighlights();
    
    /**
     * @brief Set animation enabled
     * @param enabled Animation state
     */
    void setAnimationEnabled(bool enabled);
    
    /**
     * @brief Check if animation is enabled
     * @return True if animation is enabled
     */
    bool isAnimationEnabled() const;
    
    /**
     * @brief Capture screenshot
     * @param filename Output filename
     * @return True if successful
     */
    bool captureScreenshot(const std::string& filename) const;
    
    /**
     * @brief Export visualization as SVG
     * @param filename Output filename
     * @param network Neural network to export
     * @return True if successful
     */
    bool exportSVG(const std::string& filename, const core::DefaultNetwork& network) const;

private:
    sf::RenderWindow& window_;                          ///< Reference to render window
    RenderConfig renderConfig_;                         ///< Render configuration
    ColorSchemeManager colorSchemeManager_;             ///< Color scheme manager
    RenderStats renderStats_;                           ///< Render statistics
    Viewport viewport_;                                 ///< Viewport information
    
    // Subsystems
    std::unique_ptr<NetworkRenderer> networkRenderer_;  ///< Network renderer
    std::unique_ptr<LayoutManager> layoutManager_;      ///< Layout manager
    std::unique_ptr<AnimationSystem> animationSystem_;  ///< Animation system
    
    // State
    bool initialized_;                                  ///< Initialization flag
    sf::Vector2f lastMousePos_;                         ///< Last mouse position
    bool isDragging_;                                   ///< Mouse dragging state
    std::pair<int, int> highlightedNeuron_;            ///< Highlighted neuron
    
    // Timing
    sf::Clock renderClock_;                             ///< Render timing
    float frameTime_;                                   ///< Current frame time
    
    /**
     * @brief Initialize subsystems
     * @return True if successful
     */
    bool initializeSubsystems();
    
    /**
     * @brief Update viewport
     */
    void updateViewport();
    
    /**
     * @brief Update render statistics
     */
    void updateRenderStats();
    
    /**
     * @brief Handle mouse wheel
     * @param delta Wheel delta
     * @param mousePos Mouse position
     */
    void handleMouseWheel(float delta, const sf::Vector2f& mousePos);
    
    /**
     * @brief Handle mouse drag
     * @param mousePos Current mouse position
     */
    void handleMouseDrag(const sf::Vector2f& mousePos);
    
    /**
     * @brief Calculate network bounds
     * @param network Neural network
     * @return Bounding rectangle
     */
    sf::FloatRect calculateNetworkBounds(const core::DefaultNetwork& network) const;
};

} // namespace graphics
} // namespace nnv
