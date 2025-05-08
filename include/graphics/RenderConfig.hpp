/**
 * @file RenderConfig.hpp
 * @brief Rendering configuration structures for the visualization system
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <vector>
#include <SFML/Graphics.hpp>

#include "core/Types.hpp"
#include "utils/Common.hpp"

namespace nnv {
namespace graphics {

/**
 * @brief Network layout types
 */
enum class LayoutType {
    Horizontal,
    Vertical,
    Circular,
    ForceDirected,
    Grid
};

/**
 * @brief Color scheme types
 */
enum class ColorScheme {
    Default,
    BlueGradient,
    RedGradient,
    Rainbow,
    Grayscale,
    HighContrast,
    Custom
};

/**
 * @brief Animation easing types
 */
enum class EasingType {
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
    Bounce,
    Elastic
};

/**
 * @brief Neuron rendering configuration
 */
struct NeuronRenderConfig {
    float radius = 15.0f;                    ///< Neuron radius in pixels
    sf::Color fillColor = sf::Color::White;  ///< Fill color
    sf::Color outlineColor = sf::Color::Black; ///< Outline color
    float outlineThickness = 1.0f;           ///< Outline thickness
    bool showValue = true;                   ///< Show activation value
    bool showBias = false;                   ///< Show bias value
    int valueDecimalPlaces = 2;              ///< Decimal places for values
    sf::Color textColor = sf::Color::Black;  ///< Text color
    float textSize = 12.0f;                  ///< Text size
};

/**
 * @brief Connection rendering configuration
 */
struct ConnectionRenderConfig {
    float thickness = 2.0f;                  ///< Base connection thickness
    float maxThickness = 8.0f;               ///< Maximum thickness for strong weights
    sf::Color positiveColor = sf::Color::Green; ///< Color for positive weights
    sf::Color negativeColor = sf::Color::Red;   ///< Color for negative weights
    sf::Color neutralColor = sf::Color::Gray;   ///< Color for zero weights
    float alphaMultiplier = 1.0f;            ///< Alpha transparency multiplier
    bool showWeightValues = false;           ///< Show weight values as text
    float weightThreshold = 0.01f;           ///< Minimum weight to display
    bool animateFlow = true;                 ///< Animate data flow
    float flowSpeed = 1.0f;                  ///< Flow animation speed
};

/**
 * @brief Layer rendering configuration
 */
struct LayerRenderConfig {
    sf::Vector2f position = {0.0f, 0.0f};    ///< Layer position
    float spacing = 200.0f;                  ///< Spacing between layers
    float neuronSpacing = 40.0f;             ///< Spacing between neurons in layer
    std::string label;                       ///< Layer label
    sf::Color labelColor = sf::Color::Black; ///< Label color
    float labelSize = 16.0f;                 ///< Label text size
    bool showLabel = true;                   ///< Show layer label
    bool showNeuronIndices = false;          ///< Show neuron indices
};

/**
 * @brief Animation configuration
 */
struct AnimationConfig {
    bool enabled = true;                     ///< Enable animations
    float duration = 0.5f;                  ///< Animation duration in seconds
    EasingType easing = EasingType::EaseInOut; ///< Easing function
    bool smoothTransitions = true;           ///< Smooth value transitions
    float transitionSpeed = 2.0f;            ///< Transition speed multiplier
    bool highlightActiveNeurons = true;      ///< Highlight active neurons
    sf::Color highlightColor = sf::Color::Yellow; ///< Highlight color
};

/**
 * @brief Main rendering configuration
 */
struct RenderConfig {
    // Layout
    LayoutType layout = LayoutType::Horizontal;
    sf::Vector2f networkPosition = {100.0f, 100.0f}; ///< Network position on screen
    float zoom = 1.0f;                       ///< Zoom level
    sf::Vector2f pan = {0.0f, 0.0f};         ///< Pan offset
    
    // Colors and theme
    ColorScheme colorScheme = ColorScheme::Default;
    sf::Color backgroundColor = sf::Color::White;
    std::vector<sf::Color> customColors;     ///< Custom color palette
    
    // Component configurations
    NeuronRenderConfig neuron;
    ConnectionRenderConfig connection;
    LayerRenderConfig layer;
    AnimationConfig animation;
    
    // Performance
    bool enableCulling = true;               ///< Enable frustum culling
    float cullingMargin = 50.0f;             ///< Culling margin in pixels
    int maxVisibleConnections = 10000;      ///< Maximum connections to render
    bool enableLOD = true;                   ///< Enable level of detail
    float lodDistance = 500.0f;              ///< LOD switch distance
    
    // Debug
    bool showDebugInfo = false;              ///< Show debug information
    bool showBoundingBoxes = false;          ///< Show bounding boxes
    bool showPerformanceStats = false;       ///< Show performance statistics
    
    /**
     * @brief Get color for activation value
     * @param activation Activation value (0.0 to 1.0)
     * @return Color representing the activation
     */
    sf::Color getActivationColor(float activation) const;
    
    /**
     * @brief Get color for weight value
     * @param weight Weight value
     * @param maxWeight Maximum absolute weight for normalization
     * @return Color representing the weight
     */
    sf::Color getWeightColor(float weight, float maxWeight = 1.0f) const;
    
    /**
     * @brief Get neuron position for given layer and index
     * @param layerIndex Layer index
     * @param neuronIndex Neuron index within layer
     * @param layerSize Size of the layer
     * @param totalLayers Total number of layers
     * @return Position of the neuron
     */
    sf::Vector2f getNeuronPosition(std::size_t layerIndex, 
                                  std::size_t neuronIndex,
                                  std::size_t layerSize,
                                  std::size_t totalLayers) const;
    
    /**
     * @brief Apply color scheme to configuration
     * @param scheme Color scheme to apply
     */
    void applyColorScheme(ColorScheme scheme);
    
    /**
     * @brief Load configuration from JSON
     * @param json JSON object
     */
    void fromJson(const nlohmann::json& json);
    
    /**
     * @brief Save configuration to JSON
     * @return JSON object
     */
    nlohmann::json toJson() const;
};

/**
 * @brief Rendering statistics
 */
struct RenderStats {
    int neuronsRendered = 0;                 ///< Number of neurons rendered
    int connectionsRendered = 0;             ///< Number of connections rendered
    float renderTime = 0.0f;                 ///< Render time in milliseconds
    float fps = 0.0f;                        ///< Frames per second
    int drawCalls = 0;                       ///< Number of draw calls
    std::size_t memoryUsage = 0;             ///< Memory usage in bytes
    
    /**
     * @brief Reset statistics
     */
    void reset() {
        neuronsRendered = 0;
        connectionsRendered = 0;
        renderTime = 0.0f;
        drawCalls = 0;
    }
};

/**
 * @brief Viewport information
 */
struct Viewport {
    sf::FloatRect bounds;                    ///< Viewport bounds
    sf::Vector2f center;                     ///< Viewport center
    float zoom = 1.0f;                       ///< Zoom level
    
    /**
     * @brief Check if point is visible in viewport
     * @param point Point to check
     * @return True if visible
     */
    bool isVisible(const sf::Vector2f& point) const {
        return bounds.contains(point);
    }
    
    /**
     * @brief Check if rectangle is visible in viewport
     * @param rect Rectangle to check
     * @return True if visible
     */
    bool isVisible(const sf::FloatRect& rect) const {
        return bounds.intersects(rect);
    }
    
    /**
     * @brief Transform world coordinates to screen coordinates
     * @param worldPos World position
     * @return Screen position
     */
    sf::Vector2f worldToScreen(const sf::Vector2f& worldPos) const {
        return (worldPos - center) * zoom + sf::Vector2f(bounds.width / 2, bounds.height / 2);
    }
    
    /**
     * @brief Transform screen coordinates to world coordinates
     * @param screenPos Screen position
     * @return World position
     */
    sf::Vector2f screenToWorld(const sf::Vector2f& screenPos) const {
        return (screenPos - sf::Vector2f(bounds.width / 2, bounds.height / 2)) / zoom + center;
    }
};

} // namespace graphics
} // namespace nnv
