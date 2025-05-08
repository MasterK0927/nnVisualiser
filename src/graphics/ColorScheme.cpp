/**
 * @file ColorScheme.cpp
 * @brief Implementation of color scheme management
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include "graphics/ColorScheme.hpp"
#include <algorithm>
#include <cmath>

namespace nnv {
namespace graphics {

namespace color {

sf::Color lerp(const sf::Color& color1, const sf::Color& color2, float t) {
    t = std::max(0.0f, std::min(1.0f, t));
    
    return sf::Color(
        static_cast<sf::Uint8>(color1.r + t * (color2.r - color1.r)),
        static_cast<sf::Uint8>(color1.g + t * (color2.g - color1.g)),
        static_cast<sf::Uint8>(color1.b + t * (color2.b - color1.b)),
        static_cast<sf::Uint8>(color1.a + t * (color2.a - color1.a))
    );
}

sf::Color hsvToRgb(float h, float s, float v) {
    h = std::fmod(h, 360.0f);
    if (h < 0.0f) h += 360.0f;
    
    s = std::max(0.0f, std::min(1.0f, s));
    v = std::max(0.0f, std::min(1.0f, v));
    
    if (s <= 0.0f) {
        return sf::Color(
            static_cast<sf::Uint8>(v * 255),
            static_cast<sf::Uint8>(v * 255),
            static_cast<sf::Uint8>(v * 255)
        );
    }
    
    float hh = h / 60.0f;
    int i = static_cast<int>(hh);
    float ff = hh - i;
    
    float p = v * (1.0f - s);
    float q = v * (1.0f - (s * ff));
    float t = v * (1.0f - (s * (1.0f - ff)));
    
    float r, g, b;
    
    switch (i) {
        case 0:
            r = v; g = t; b = p;
            break;
        case 1:
            r = q; g = v; b = p;
            break;
        case 2:
            r = p; g = v; b = t;
            break;
        case 3:
            r = p; g = q; b = v;
            break;
        case 4:
            r = t; g = p; b = v;
            break;
        default:
            r = v; g = p; b = q;
            break;
    }
    
    return sf::Color(
        static_cast<sf::Uint8>(r * 255),
        static_cast<sf::Uint8>(g * 255),
        static_cast<sf::Uint8>(b * 255)
    );
}

std::vector<float> rgbToHsv(const sf::Color& color) {
    float r = color.r / 255.0f;
    float g = color.g / 255.0f;
    float b = color.b / 255.0f;
    
    float max = std::max(std::max(r, g), b);
    float min = std::min(std::min(r, g), b);
    float delta = max - min;
    
    float h = 0.0f;
    float s = (max > 0.0f) ? (delta / max) : 0.0f;
    float v = max;
    
    if (delta > 0.0f) {
        if (max == r) {
            h = 60.0f * std::fmod((g - b) / delta, 6.0f);
        } else if (max == g) {
            h = 60.0f * ((b - r) / delta + 2.0f);
        } else {
            h = 60.0f * ((r - g) / delta + 4.0f);
        }
    }
    
    if (h < 0.0f) h += 360.0f;
    
    return {h, s, v};
}

sf::Color adjustBrightness(const sf::Color& color, float factor) {
    auto hsv = rgbToHsv(color);
    hsv[2] *= factor; // Adjust value (brightness)
    return hsvToRgb(hsv[0], hsv[1], hsv[2]);
}

sf::Color adjustSaturation(const sf::Color& color, float factor) {
    auto hsv = rgbToHsv(color);
    hsv[1] *= factor; // Adjust saturation
    return hsvToRgb(hsv[0], hsv[1], hsv[2]);
}

sf::Color getContrastingColor(const sf::Color& color) {
    // Calculate perceived brightness using the formula:
    // (0.299*R + 0.587*G + 0.114*B)
    float brightness = (0.299f * color.r + 0.587f * color.g + 0.114f * color.b) / 255.0f;
    
    // Use white for dark colors, black for light colors
    return (brightness < 0.5f) ? sf::Color::White : sf::Color::Black;
}

std::vector<sf::Color> generateGradient(const sf::Color& startColor, 
                                       const sf::Color& endColor, 
                                       int steps) {
    std::vector<sf::Color> gradient;
    gradient.reserve(steps);
    
    for (int i = 0; i < steps; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(steps - 1);
        gradient.push_back(lerp(startColor, endColor, t));
    }
    
    return gradient;
}

} // namespace color

ColorSchemeManager::ColorSchemeManager() {
    initializePredefinedSchemes();
}

std::vector<sf::Color> ColorSchemeManager::getScheme(ColorScheme scheme) const {
    switch (scheme) {
        case ColorScheme::Default:
            return getDefaultScheme();
        case ColorScheme::BlueGradient:
            return getBlueGradientScheme();
        case ColorScheme::RedGradient:
            return getRedGradientScheme();
        case ColorScheme::Rainbow:
            return getRainbowScheme();
        case ColorScheme::Grayscale:
            return getGrayscaleScheme();
        case ColorScheme::HighContrast:
            return getHighContrastScheme();
        case ColorScheme::Custom:
            if (!customSchemes_.empty()) {
                return customSchemes_.begin()->second;
            }
            return getDefaultScheme();
        default:
            return getDefaultScheme();
    }
}

sf::Color ColorSchemeManager::getActivationColor(float activation, ColorScheme scheme) const {
    activation = std::max(0.0f, std::min(1.0f, activation));
    
    auto colors = getScheme(scheme);
    if (colors.empty()) {
        return sf::Color::White;
    }
    
    if (colors.size() == 1) {
        return color::adjustBrightness(colors[0], activation);
    }
    
    float index = activation * (colors.size() - 1);
    int lowerIndex = static_cast<int>(index);
    int upperIndex = std::min(lowerIndex + 1, static_cast<int>(colors.size() - 1));
    float t = index - lowerIndex;
    
    return color::lerp(colors[lowerIndex], colors[upperIndex], t);
}

sf::Color ColorSchemeManager::getWeightColor(float weight, float maxWeight, ColorScheme scheme) const {
    // Normalize weight to [-1, 1] range
    float normalizedWeight = (maxWeight > 0.0f) ? (weight / maxWeight) : weight;
    normalizedWeight = std::max(-1.0f, std::min(1.0f, normalizedWeight));
    
    // Map to [0, 1] for color interpolation
    float t = (normalizedWeight + 1.0f) * 0.5f;
    
    auto colors = getScheme(scheme);
    if (colors.empty()) {
        return sf::Color::White;
    }
    
    if (colors.size() == 1) {
        return colors[0];
    }
    
    // For weights, use a different mapping:
    // Negative weights: first color to middle color
    // Positive weights: middle color to last color
    if (normalizedWeight < 0.0f) {
        return color::lerp(colors.front(), colors[colors.size() / 2], t * 2.0f);
    } else {
        return color::lerp(colors[colors.size() / 2], colors.back(), t * 2.0f - 1.0f);
    }
}

sf::Color ColorSchemeManager::getNeuronColor(float activation, ColorScheme scheme) const {
    return getActivationColor(activation, scheme);
}

sf::Color ColorSchemeManager::getLayerColor(std::size_t layerIndex, std::size_t totalLayers, ColorScheme scheme) const {
    if (totalLayers <= 1) {
        return getScheme(scheme).front();
    }
    
    float t = static_cast<float>(layerIndex) / static_cast<float>(totalLayers - 1);
    auto colors = getScheme(scheme);
    
    float index = t * (colors.size() - 1);
    int lowerIndex = static_cast<int>(index);
    int upperIndex = std::min(lowerIndex + 1, static_cast<int>(colors.size() - 1));
    float interpolation = index - lowerIndex;
    
    return color::lerp(colors[lowerIndex], colors[upperIndex], interpolation);
}

void ColorSchemeManager::registerCustomScheme(const std::string& name, const std::vector<sf::Color>& colors) {
    if (!colors.empty()) {
        customSchemes_[name] = colors;
    }
}

std::vector<sf::Color> ColorSchemeManager::getCustomScheme(const std::string& name) const {
    auto it = customSchemes_.find(name);
    if (it != customSchemes_.end()) {
        return it->second;
    }
    return {};
}

std::vector<std::string> ColorSchemeManager::getAvailableSchemes() const {
    std::vector<std::string> schemes = {
        "Default", "BlueGradient", "RedGradient", "Rainbow", "Grayscale", "HighContrast"
    };
    
    for (const auto& pair : customSchemes_) {
        schemes.push_back(pair.first);
    }
    
    return schemes;
}

void ColorSchemeManager::applyScheme(RenderConfig& config, ColorScheme scheme) const {
    auto colors = getScheme(scheme);
    if (colors.empty()) {
        return;
    }
    
    // Set background color
    config.backgroundColor = sf::Color::White;
    
    // Set neuron colors
    config.neuron.fillColor = colors[0];
    config.neuron.outlineColor = color::adjustBrightness(colors[0], 0.7f);
    config.neuron.textColor = color::getContrastingColor(colors[0]);
    
    // Set connection colors
    if (colors.size() >= 3) {
        config.connection.negativeColor = colors[0];
        config.connection.neutralColor = colors[colors.size() / 2];
        config.connection.positiveColor = colors.back();
    } else {
        config.connection.negativeColor = sf::Color::Red;
        config.connection.neutralColor = sf::Color::Gray;
        config.connection.positiveColor = sf::Color::Green;
    }
    
    // Store custom colors
    config.customColors = colors;
}

void ColorSchemeManager::initializePredefinedSchemes() {
    // Register some built-in custom schemes
    registerCustomScheme("Viridis", {
        sf::Color(68, 1, 84),
        sf::Color(72, 40, 120),
        sf::Color(62, 73, 137),
        sf::Color(49, 104, 142),
        sf::Color(38, 130, 142),
        sf::Color(31, 158, 137),
        sf::Color(53, 183, 121),
        sf::Color(109, 205, 89),
        sf::Color(180, 222, 44),
        sf::Color(253, 231, 37)
    });
    
    registerCustomScheme("Plasma", {
        sf::Color(13, 8, 135),
        sf::Color(84, 2, 163),
        sf::Color(139, 10, 165),
        sf::Color(185, 50, 137),
        sf::Color(219, 92, 104),
        sf::Color(244, 136, 73),
        sf::Color(254, 188, 43),
        sf::Color(240, 249, 33)
    });
    
    registerCustomScheme("Neural", {
        sf::Color(50, 50, 50),    // Dark gray
        sf::Color(100, 149, 237), // Cornflower blue
        sf::Color(255, 127, 80),  // Coral
        sf::Color(50, 205, 50)    // Lime green
    });
}

std::vector<sf::Color> ColorSchemeManager::getDefaultScheme() const {
    return {
        sf::Color(65, 105, 225),  // Royal blue
        sf::Color(100, 149, 237), // Cornflower blue
        sf::Color(135, 206, 250), // Light sky blue
        sf::Color(173, 216, 230), // Light blue
        sf::Color(240, 248, 255)  // Alice blue
    };
}

std::vector<sf::Color> ColorSchemeManager::getBlueGradientScheme() const {
    return {
        sf::Color(0, 0, 139),     // Dark blue
        sf::Color(0, 0, 205),     // Medium blue
        sf::Color(0, 0, 255),     // Blue
        sf::Color(30, 144, 255),  // Dodger blue
        sf::Color(135, 206, 250)  // Light sky blue
    };
}

std::vector<sf::Color> ColorSchemeManager::getRedGradientScheme() const {
    return {
        sf::Color(139, 0, 0),     // Dark red
        sf::Color(178, 34, 34),   // Firebrick
        sf::Color(205, 92, 92),   // Indian red
        sf::Color(240, 128, 128), // Light coral
        sf::Color(255, 160, 122)  // Light salmon
    };
}

std::vector<sf::Color> ColorSchemeManager::getRainbowScheme() const {
    return {
        sf::Color(255, 0, 0),     // Red
        sf::Color(255, 127, 0),   // Orange
        sf::Color(255, 255, 0),   // Yellow
        sf::Color(0, 255, 0),     // Green
        sf::Color(0, 0, 255),     // Blue
        sf::Color(75, 0, 130),    // Indigo
        sf::Color(148, 0, 211)    // Violet
    };
}

std::vector<sf::Color> ColorSchemeManager::getGrayscaleScheme() const {
    return {
        sf::Color(25, 25, 25),    // Almost black
        sf::Color(75, 75, 75),    // Dark gray
        sf::Color(128, 128, 128), // Gray
        sf::Color(192, 192, 192), // Light gray
        sf::Color(245, 245, 245)  // Almost white
    };
}

std::vector<sf::Color> ColorSchemeManager::getHighContrastScheme() const {
    return {
        sf::Color(0, 0, 0),       // Black
        sf::Color(255, 255, 0),   // Yellow
        sf::Color(0, 255, 255),   // Cyan
        sf::Color(255, 0, 255),   // Magenta
        sf::Color(255, 255, 255)  // White
    };
}

} // namespace graphics
} // namespace nnv
