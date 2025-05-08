/**
 * @file ColorScheme.hpp
 * @brief Color scheme management for the visualization system
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <SFML/Graphics.hpp>

#include "graphics/RenderConfig.hpp"
#include "utils/Common.hpp"

namespace nnv {
namespace graphics {

/**
 * @brief Color utility functions
 */
namespace color {

/**
 * @brief Interpolate between two colors
 * @param color1 First color
 * @param color2 Second color
 * @param t Interpolation factor (0.0 to 1.0)
 * @return Interpolated color
 */
sf::Color lerp(const sf::Color& color1, const sf::Color& color2, float t);

/**
 * @brief Convert HSV to RGB color
 * @param h Hue (0-360)
 * @param s Saturation (0-1)
 * @param v Value (0-1)
 * @return RGB color
 */
sf::Color hsvToRgb(float h, float s, float v);

/**
 * @brief Convert RGB to HSV color
 * @param color RGB color
 * @return HSV values as {h, s, v}
 */
std::vector<float> rgbToHsv(const sf::Color& color);

/**
 * @brief Adjust color brightness
 * @param color Input color
 * @param factor Brightness factor (1.0 = no change)
 * @return Adjusted color
 */
sf::Color adjustBrightness(const sf::Color& color, float factor);

/**
 * @brief Adjust color saturation
 * @param color Input color
 * @param factor Saturation factor (1.0 = no change)
 * @return Adjusted color
 */
sf::Color adjustSaturation(const sf::Color& color, float factor);

/**
 * @brief Get contrasting color (black or white)
 * @param color Input color
 * @return Black or white for best contrast
 */
sf::Color getContrastingColor(const sf::Color& color);

/**
 * @brief Generate gradient colors
 * @param startColor Start color
 * @param endColor End color
 * @param steps Number of gradient steps
 * @return Vector of gradient colors
 */
std::vector<sf::Color> generateGradient(const sf::Color& startColor, 
                                       const sf::Color& endColor, 
                                       int steps);

} // namespace color

/**
 * @brief Color scheme manager
 */
class ColorSchemeManager {
public:
    /**
     * @brief Constructor
     */
    ColorSchemeManager();
    
    /**
     * @brief Get predefined color scheme
     * @param scheme Color scheme type
     * @return Vector of colors
     */
    std::vector<sf::Color> getScheme(ColorScheme scheme) const;
    
    /**
     * @brief Get activation color for value
     * @param activation Activation value (0.0 to 1.0)
     * @param scheme Color scheme
     * @return Color representing activation
     */
    sf::Color getActivationColor(float activation, ColorScheme scheme) const;
    
    /**
     * @brief Get weight color for value
     * @param weight Weight value
     * @param maxWeight Maximum absolute weight
     * @param scheme Color scheme
     * @return Color representing weight
     */
    sf::Color getWeightColor(float weight, float maxWeight, ColorScheme scheme) const;
    
    /**
     * @brief Get neuron color based on activation
     * @param activation Activation value
     * @param scheme Color scheme
     * @return Neuron color
     */
    sf::Color getNeuronColor(float activation, ColorScheme scheme) const;
    
    /**
     * @brief Get layer color
     * @param layerIndex Layer index
     * @param totalLayers Total number of layers
     * @param scheme Color scheme
     * @return Layer color
     */
    sf::Color getLayerColor(std::size_t layerIndex, std::size_t totalLayers, ColorScheme scheme) const;
    
    /**
     * @brief Register custom color scheme
     * @param name Scheme name
     * @param colors Vector of colors
     */
    void registerCustomScheme(const std::string& name, const std::vector<sf::Color>& colors);
    
    /**
     * @brief Get custom color scheme
     * @param name Scheme name
     * @return Vector of colors (empty if not found)
     */
    std::vector<sf::Color> getCustomScheme(const std::string& name) const;
    
    /**
     * @brief Get all available scheme names
     * @return Vector of scheme names
     */
    std::vector<std::string> getAvailableSchemes() const;
    
    /**
     * @brief Apply color scheme to render configuration
     * @param config Render configuration to modify
     * @param scheme Color scheme to apply
     */
    void applyScheme(RenderConfig& config, ColorScheme scheme) const;

private:
    std::unordered_map<std::string, std::vector<sf::Color>> customSchemes_;
    
    /**
     * @brief Initialize predefined color schemes
     */
    void initializePredefinedSchemes();
    
    /**
     * @brief Get default color scheme
     * @return Default colors
     */
    std::vector<sf::Color> getDefaultScheme() const;
    
    /**
     * @brief Get blue gradient scheme
     * @return Blue gradient colors
     */
    std::vector<sf::Color> getBlueGradientScheme() const;
    
    /**
     * @brief Get red gradient scheme
     * @return Red gradient colors
     */
    std::vector<sf::Color> getRedGradientScheme() const;
    
    /**
     * @brief Get rainbow scheme
     * @return Rainbow colors
     */
    std::vector<sf::Color> getRainbowScheme() const;
    
    /**
     * @brief Get grayscale scheme
     * @return Grayscale colors
     */
    std::vector<sf::Color> getGrayscaleScheme() const;
    
    /**
     * @brief Get high contrast scheme
     * @return High contrast colors
     */
    std::vector<sf::Color> getHighContrastScheme() const;
};

} // namespace graphics
} // namespace nnv
