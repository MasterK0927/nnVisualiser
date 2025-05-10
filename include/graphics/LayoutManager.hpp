/**
 * @file LayoutManager.hpp
 * @brief Layout management for neural network visualization
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

#include "core/Types.hpp"
#include "graphics/RenderConfig.hpp"
#include "utils/Common.hpp"

namespace nnv {
namespace core {
    template<typename T> class NeuralNetwork;
}
}

namespace nnv {
namespace graphics {

/**
 * @brief Layout information for a neuron
 */
struct NeuronLayout {
    sf::Vector2f position;          ///< Neuron position
    float radius;                   ///< Neuron radius
    std::size_t layerIndex;         ///< Layer index
    std::size_t neuronIndex;        ///< Neuron index within layer
};

/**
 * @brief Layout information for a layer
 */
struct LayerLayout {
    sf::Vector2f position;          ///< Layer center position
    sf::FloatRect bounds;           ///< Layer bounding box
    std::vector<NeuronLayout> neurons; ///< Neuron layouts
    std::string label;              ///< Layer label
    std::size_t layerIndex;         ///< Layer index
};

/**
 * @brief Complete network layout
 */
struct NetworkLayout {
    std::vector<LayerLayout> layers; ///< Layer layouts
    sf::FloatRect bounds;           ///< Overall network bounds
    sf::Vector2f center;            ///< Network center
    LayoutType type;                ///< Layout type used
    
    /**
     * @brief Get neuron layout
     * @param layerIndex Layer index
     * @param neuronIndex Neuron index
     * @return Neuron layout or nullptr if not found
     */
    const NeuronLayout* getNeuronLayout(std::size_t layerIndex, std::size_t neuronIndex) const;
    
    /**
     * @brief Find neuron at position
     * @param position Screen position
     * @param tolerance Search tolerance
     * @return Neuron indices (layer, neuron) or {-1, -1} if none
     */
    std::pair<int, int> findNeuronAtPosition(const sf::Vector2f& position, float tolerance = 5.0f) const;
};

/**
 * @brief Layout manager for different network arrangements
 */
class LayoutManager {
public:
    /**
     * @brief Constructor
     * @param config Render configuration
     */
    explicit LayoutManager(const RenderConfig& config);
    
    /**
     * @brief Destructor
     */
    ~LayoutManager() = default;
    
    // Disable copy and move
    NNV_DISABLE_COPY_AND_MOVE(LayoutManager)
    
    /**
     * @brief Calculate layout for network
     * @param network Neural network
     * @param viewport Target viewport
     * @return Network layout
     */
    NetworkLayout calculateLayout(const core::DefaultNetwork& network, 
                                 const Viewport& viewport) const;
    
    /**
     * @brief Set render configuration
     * @param config New render configuration
     */
    void setRenderConfig(const RenderConfig& config) { config_ = config; }
    
    /**
     * @brief Get render configuration
     * @return Current render configuration
     */
    const RenderConfig& getRenderConfig() const { return config_; }
    
    /**
     * @brief Calculate optimal spacing for layout
     * @param network Neural network
     * @param viewport Target viewport
     * @return Optimal spacing values
     */
    std::pair<float, float> calculateOptimalSpacing(const core::DefaultNetwork& network,
                                                   const Viewport& viewport) const;
    
    /**
     * @brief Fit layout to viewport
     * @param layout Network layout to modify
     * @param viewport Target viewport
     * @param margin Margin around edges
     */
    void fitToViewport(NetworkLayout& layout, const Viewport& viewport, float margin = 50.0f) const;

private:
    RenderConfig config_;           ///< Render configuration
    
    /**
     * @brief Calculate horizontal layout
     * @param network Neural network
     * @param viewport Target viewport
     * @return Network layout
     */
    NetworkLayout calculateHorizontalLayout(const core::DefaultNetwork& network,
                                           const Viewport& viewport) const;
    
    /**
     * @brief Calculate vertical layout
     * @param network Neural network
     * @param viewport Target viewport
     * @return Network layout
     */
    NetworkLayout calculateVerticalLayout(const core::DefaultNetwork& network,
                                         const Viewport& viewport) const;
    
    /**
     * @brief Calculate circular layout
     * @param network Neural network
     * @param viewport Target viewport
     * @return Network layout
     */
    NetworkLayout calculateCircularLayout(const core::DefaultNetwork& network,
                                         const Viewport& viewport) const;
    
    /**
     * @brief Calculate grid layout
     * @param network Neural network
     * @param viewport Target viewport
     * @return Network layout
     */
    NetworkLayout calculateGridLayout(const core::DefaultNetwork& network,
                                     const Viewport& viewport) const;
    
    /**
     * @brief Calculate force-directed layout
     * @param network Neural network
     * @param viewport Target viewport
     * @return Network layout
     */
    NetworkLayout calculateForceDirectedLayout(const core::DefaultNetwork& network,
                                              const Viewport& viewport) const;
    
    /**
     * @brief Calculate layer positions for horizontal layout
     * @param layerCount Number of layers
     * @param spacing Layer spacing
     * @param startX Starting X position
     * @return Vector of layer X positions
     */
    std::vector<float> calculateHorizontalLayerPositions(std::size_t layerCount,
                                                         float spacing,
                                                         float startX) const;
    
    /**
     * @brief Calculate neuron positions within a layer
     * @param layerSize Number of neurons in layer
     * @param layerCenter Layer center position
     * @param neuronSpacing Spacing between neurons
     * @param vertical Whether to arrange vertically
     * @return Vector of neuron positions
     */
    std::vector<sf::Vector2f> calculateNeuronPositions(std::size_t layerSize,
                                                       const sf::Vector2f& layerCenter,
                                                       float neuronSpacing,
                                                       bool vertical = true) const;
    
    /**
     * @brief Calculate circular positions
     * @param count Number of positions
     * @param center Circle center
     * @param radius Circle radius
     * @param startAngle Starting angle in degrees
     * @return Vector of positions
     */
    std::vector<sf::Vector2f> calculateCircularPositions(std::size_t count,
                                                         const sf::Vector2f& center,
                                                         float radius,
                                                         float startAngle = 0.0f) const;
    
    /**
     * @brief Calculate grid dimensions
     * @param itemCount Number of items to arrange
     * @param aspectRatio Preferred aspect ratio (width/height)
     * @return Grid dimensions (columns, rows)
     */
    std::pair<int, int> calculateGridDimensions(std::size_t itemCount, float aspectRatio = 1.0f) const;
    
    /**
     * @brief Calculate bounding box for positions
     * @param positions Vector of positions
     * @param radius Item radius
     * @return Bounding rectangle
     */
    sf::FloatRect calculateBounds(const std::vector<sf::Vector2f>& positions, float radius = 0.0f) const;
    
    /**
     * @brief Apply force-directed algorithm step
     * @param positions Current positions (modified)
     * @param network Neural network for connectivity
     * @param repulsionStrength Repulsion force strength
     * @param attractionStrength Attraction force strength
     * @param damping Damping factor
     */
    void applyForceDirectedStep(std::vector<std::vector<sf::Vector2f>>& positions,
                               const core::DefaultNetwork& network,
                               float repulsionStrength = 1000.0f,
                               float attractionStrength = 0.1f,
                               float damping = 0.9f) const;
    
    /**
     * @brief Calculate repulsion force between two points
     * @param pos1 First position
     * @param pos2 Second position
     * @param strength Force strength
     * @return Force vector applied to pos1
     */
    sf::Vector2f calculateRepulsionForce(const sf::Vector2f& pos1,
                                        const sf::Vector2f& pos2,
                                        float strength) const;
    
    /**
     * @brief Calculate attraction force between connected neurons
     * @param pos1 First position
     * @param pos2 Second position
     * @param weight Connection weight
     * @param strength Force strength
     * @return Force vector applied to pos1
     */
    sf::Vector2f calculateAttractionForce(const sf::Vector2f& pos1,
                                         const sf::Vector2f& pos2,
                                         float weight,
                                         float strength) const;
    
    /**
     * @brief Normalize layout to fit within bounds
     * @param layout Network layout to normalize
     * @param targetBounds Target bounds
     * @param margin Margin around edges
     */
    void normalizeLayout(NetworkLayout& layout, 
                        const sf::FloatRect& targetBounds,
                        float margin = 50.0f) const;
};

} // namespace graphics
} // namespace nnv
