/**
 * @file NetworkRenderer.hpp
 * @brief Neural network rendering system
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

// Forward declarations
namespace nnv {
namespace core {
    template<typename T> class NeuralNetwork;
}
namespace graphics {
    class LayerRenderer;
    class NeuronRenderer;
    class ConnectionRenderer;
}
}

namespace nnv {
namespace graphics {

/**
 * @brief Rendered neuron information
 */
struct RenderedNeuron {
    sf::Vector2f position;          ///< Screen position
    float radius;                   ///< Render radius
    sf::Color color;                ///< Render color
    float activation;               ///< Activation value
    bool visible;                   ///< Visibility flag
    bool highlighted;               ///< Highlight flag
};

/**
 * @brief Rendered connection information
 */
struct RenderedConnection {
    sf::Vector2f startPos;          ///< Start position
    sf::Vector2f endPos;            ///< End position
    float thickness;                ///< Line thickness
    sf::Color color;                ///< Line color
    float weight;                   ///< Weight value
    bool visible;                   ///< Visibility flag
    float animationPhase;           ///< Animation phase (0-1)
};

/**
 * @brief Rendered layer information
 */
struct RenderedLayer {
    std::vector<RenderedNeuron> neurons;    ///< Layer neurons
    sf::Vector2f position;                  ///< Layer position
    sf::FloatRect bounds;                   ///< Layer bounds
    std::string label;                      ///< Layer label
    bool visible;                           ///< Visibility flag
};

/**
 * @brief Main network renderer class
 */
class NetworkRenderer {
public:
    /**
     * @brief Constructor
     * @param config Render configuration
     */
    explicit NetworkRenderer(const RenderConfig& config);
    
    /**
     * @brief Destructor
     */
    ~NetworkRenderer();
    
    // Disable copy and move
    NNV_DISABLE_COPY_AND_MOVE(NetworkRenderer)
    
    /**
     * @brief Initialize renderer
     * @return True if successful
     */
    bool initialize();
    
    /**
     * @brief Update renderer
     * @param deltaTime Time since last update
     * @param network Neural network to render
     * @param viewport Current viewport
     */
    void update(float deltaTime, 
                const core::DefaultNetwork& network,
                const Viewport& viewport);
    
    /**
     * @brief Render network
     * @param target Render target
     * @param network Neural network to render
     * @param viewport Current viewport
     * @param stats Render statistics to update
     */
    void render(sf::RenderTarget& target,
                const core::DefaultNetwork& network,
                const Viewport& viewport,
                RenderStats& stats);
    
    /**
     * @brief Set render configuration
     * @param config New render configuration
     */
    void setRenderConfig(const RenderConfig& config);
    
    /**
     * @brief Get render configuration
     * @return Current render configuration
     */
    const RenderConfig& getRenderConfig() const { return config_; }
    
    /**
     * @brief Get rendered layers
     * @return Vector of rendered layers
     */
    const std::vector<RenderedLayer>& getRenderedLayers() const { return renderedLayers_; }
    
    /**
     * @brief Get rendered connections
     * @return Vector of rendered connections
     */
    const std::vector<std::vector<RenderedConnection>>& getRenderedConnections() const { 
        return renderedConnections_; 
    }
    
    /**
     * @brief Find neuron at position
     * @param position Screen position
     * @return Neuron indices (layer, neuron) or {-1, -1} if none
     */
    std::pair<int, int> findNeuronAtPosition(const sf::Vector2f& position) const;
    
    /**
     * @brief Set neuron highlight
     * @param layerIndex Layer index (-1 to clear)
     * @param neuronIndex Neuron index
     */
    void setNeuronHighlight(int layerIndex, int neuronIndex);
    
    /**
     * @brief Clear all highlights
     */
    void clearHighlights();
    
    /**
     * @brief Calculate network bounds
     * @param network Neural network
     * @return Bounding rectangle
     */
    sf::FloatRect calculateBounds(const core::DefaultNetwork& network) const;

private:
    RenderConfig config_;                               ///< Render configuration
    
    // Rendered data
    std::vector<RenderedLayer> renderedLayers_;         ///< Rendered layers
    std::vector<std::vector<RenderedConnection>> renderedConnections_; ///< Rendered connections
    
    // Subsystems
    std::unique_ptr<LayerRenderer> layerRenderer_;      ///< Layer renderer
    std::unique_ptr<NeuronRenderer> neuronRenderer_;    ///< Neuron renderer
    std::unique_ptr<ConnectionRenderer> connectionRenderer_; ///< Connection renderer
    
    // State
    bool initialized_;                                  ///< Initialization flag
    std::pair<int, int> highlightedNeuron_;            ///< Highlighted neuron
    float animationTime_;                               ///< Animation time
    
    // Caching
    bool layoutDirty_;                                  ///< Layout needs update
    sf::FloatRect cachedBounds_;                        ///< Cached network bounds
    
    /**
     * @brief Initialize subsystems
     * @return True if successful
     */
    bool initializeSubsystems();
    
    /**
     * @brief Update layout
     * @param network Neural network
     * @param viewport Current viewport
     */
    void updateLayout(const core::DefaultNetwork& network, const Viewport& viewport);
    
    /**
     * @brief Update neuron positions
     * @param network Neural network
     */
    void updateNeuronPositions(const core::DefaultNetwork& network);
    
    /**
     * @brief Update neuron colors
     * @param network Neural network
     */
    void updateNeuronColors(const core::DefaultNetwork& network);
    
    /**
     * @brief Update connections
     * @param network Neural network
     */
    void updateConnections(const core::DefaultNetwork& network);
    
    /**
     * @brief Update animations
     * @param deltaTime Time since last update
     */
    void updateAnimations(float deltaTime);
    
    /**
     * @brief Perform frustum culling
     * @param viewport Current viewport
     */
    void performCulling(const Viewport& viewport);
    
    /**
     * @brief Apply level of detail
     * @param viewport Current viewport
     */
    void applyLevelOfDetail(const Viewport& viewport);
    
    /**
     * @brief Calculate neuron position
     * @param layerIndex Layer index
     * @param neuronIndex Neuron index
     * @param layerSize Layer size
     * @param totalLayers Total number of layers
     * @return Neuron position
     */
    sf::Vector2f calculateNeuronPosition(std::size_t layerIndex,
                                        std::size_t neuronIndex,
                                        std::size_t layerSize,
                                        std::size_t totalLayers) const;
    
    /**
     * @brief Calculate layer position
     * @param layerIndex Layer index
     * @param totalLayers Total number of layers
     * @return Layer position
     */
    sf::Vector2f calculateLayerPosition(std::size_t layerIndex, std::size_t totalLayers) const;
    
    /**
     * @brief Get neuron color
     * @param activation Activation value
     * @param highlighted Whether neuron is highlighted
     * @return Neuron color
     */
    sf::Color getNeuronColor(float activation, bool highlighted) const;
    
    /**
     * @brief Get connection color
     * @param weight Connection weight
     * @param maxWeight Maximum weight for normalization
     * @return Connection color
     */
    sf::Color getConnectionColor(float weight, float maxWeight) const;
    
    /**
     * @brief Check if neuron is visible
     * @param position Neuron position
     * @param radius Neuron radius
     * @param viewport Current viewport
     * @return True if visible
     */
    bool isNeuronVisible(const sf::Vector2f& position, float radius, const Viewport& viewport) const;
    
    /**
     * @brief Check if connection is visible
     * @param startPos Connection start position
     * @param endPos Connection end position
     * @param viewport Current viewport
     * @return True if visible
     */
    bool isConnectionVisible(const sf::Vector2f& startPos, 
                           const sf::Vector2f& endPos, 
                           const Viewport& viewport) const;
};

} // namespace graphics
} // namespace nnv
