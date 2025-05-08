/**
 * @file RenderConfig.cpp
 * @brief Implementation of rendering configuration
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include "graphics/RenderConfig.hpp"
#include "graphics/ColorScheme.hpp"
#include <cmath>
#include <algorithm>

namespace nnv {
namespace graphics {

sf::Color RenderConfig::getActivationColor(float activation) const {
    ColorSchemeManager colorManager;
    return colorManager.getActivationColor(activation, colorScheme);
}

sf::Color RenderConfig::getWeightColor(float weight, float maxWeight) const {
    ColorSchemeManager colorManager;
    return colorManager.getWeightColor(weight, maxWeight, colorScheme);
}

sf::Vector2f RenderConfig::getNeuronPosition(std::size_t layerIndex, 
                                            std::size_t neuronIndex,
                                            std::size_t layerSize,
                                            std::size_t totalLayers) const {
    sf::Vector2f position = networkPosition;
    
    switch (layout) {
        case LayoutType::Horizontal: {
            // Horizontal layout: layers spread horizontally, neurons vertically
            float layerX = position.x + layerIndex * layer.spacing;
            float layerCenterY = position.y;
            
            // Center neurons vertically
            float totalHeight = (layerSize - 1) * layer.neuronSpacing;
            float startY = layerCenterY - totalHeight * 0.5f;
            float neuronY = startY + neuronIndex * layer.neuronSpacing;
            
            return sf::Vector2f(layerX, neuronY);
        }
        
        case LayoutType::Vertical: {
            // Vertical layout: layers spread vertically, neurons horizontally
            float layerY = position.y + layerIndex * layer.spacing;
            float layerCenterX = position.x;
            
            // Center neurons horizontally
            float totalWidth = (layerSize - 1) * layer.neuronSpacing;
            float startX = layerCenterX - totalWidth * 0.5f;
            float neuronX = startX + neuronIndex * layer.neuronSpacing;
            
            return sf::Vector2f(neuronX, layerY);
        }
        
        case LayoutType::Circular: {
            // Circular layout: layers as concentric circles
            float radius = 100.0f + layerIndex * layer.spacing;
            float angleStep = 2.0f * M_PI / layerSize;
            float angle = neuronIndex * angleStep;
            
            float x = position.x + radius * std::cos(angle);
            float y = position.y + radius * std::sin(angle);
            
            return sf::Vector2f(x, y);
        }
        
        case LayoutType::Grid: {
            // Grid layout: arrange all neurons in a grid
            int cols = static_cast<int>(std::ceil(std::sqrt(layerSize)));
            int row = neuronIndex / cols;
            int col = neuronIndex % cols;
            
            float x = position.x + layerIndex * layer.spacing + col * layer.neuronSpacing;
            float y = position.y + row * layer.neuronSpacing;
            
            return sf::Vector2f(x, y);
        }
        
        default:
            return position;
    }
}

void RenderConfig::applyColorScheme(ColorScheme scheme) {
    colorScheme = scheme;
    
    ColorSchemeManager colorManager;
    colorManager.applyScheme(*this, scheme);
}

void RenderConfig::fromJson(const nlohmann::json& json) {
    if (json.contains("layout")) {
        std::string layoutStr = json["layout"].get<std::string>();
        if (layoutStr == "horizontal") layout = LayoutType::Horizontal;
        else if (layoutStr == "vertical") layout = LayoutType::Vertical;
        else if (layoutStr == "circular") layout = LayoutType::Circular;
        else if (layoutStr == "grid") layout = LayoutType::Grid;
        else if (layoutStr == "force_directed") layout = LayoutType::ForceDirected;
    }
    
    if (json.contains("network_position")) {
        const auto& pos = json["network_position"];
        if (pos.is_array() && pos.size() >= 2) {
            networkPosition.x = pos[0].get<float>();
            networkPosition.y = pos[1].get<float>();
        }
    }
    
    if (json.contains("zoom")) {
        zoom = json["zoom"].get<float>();
    }
    
    if (json.contains("color_scheme")) {
        std::string schemeStr = json["color_scheme"].get<std::string>();
        if (schemeStr == "default") colorScheme = ColorScheme::Default;
        else if (schemeStr == "blue_gradient") colorScheme = ColorScheme::BlueGradient;
        else if (schemeStr == "red_gradient") colorScheme = ColorScheme::RedGradient;
        else if (schemeStr == "rainbow") colorScheme = ColorScheme::Rainbow;
        else if (schemeStr == "grayscale") colorScheme = ColorScheme::Grayscale;
        else if (schemeStr == "high_contrast") colorScheme = ColorScheme::HighContrast;
    }
    
    // Neuron configuration
    if (json.contains("neuron")) {
        const auto& neuronJson = json["neuron"];
        if (neuronJson.contains("radius")) {
            neuron.radius = neuronJson["radius"].get<float>();
        }
        if (neuronJson.contains("show_value")) {
            neuron.showValue = neuronJson["show_value"].get<bool>();
        }
        if (neuronJson.contains("show_bias")) {
            neuron.showBias = neuronJson["show_bias"].get<bool>();
        }
    }
    
    // Connection configuration
    if (json.contains("connection")) {
        const auto& connJson = json["connection"];
        if (connJson.contains("thickness")) {
            connection.thickness = connJson["thickness"].get<float>();
        }
        if (connJson.contains("show_weight_values")) {
            connection.showWeightValues = connJson["show_weight_values"].get<bool>();
        }
        if (connJson.contains("animate_flow")) {
            connection.animateFlow = connJson["animate_flow"].get<bool>();
        }
    }
    
    // Layer configuration
    if (json.contains("layer")) {
        const auto& layerJson = json["layer"];
        if (layerJson.contains("spacing")) {
            layer.spacing = layerJson["spacing"].get<float>();
        }
        if (layerJson.contains("neuron_spacing")) {
            layer.neuronSpacing = layerJson["neuron_spacing"].get<float>();
        }
        if (layerJson.contains("show_label")) {
            layer.showLabel = layerJson["show_label"].get<bool>();
        }
    }
    
    // Animation configuration
    if (json.contains("animation")) {
        const auto& animJson = json["animation"];
        if (animJson.contains("enabled")) {
            animation.enabled = animJson["enabled"].get<bool>();
        }
        if (animJson.contains("duration")) {
            animation.duration = animJson["duration"].get<float>();
        }
        if (animJson.contains("smooth_transitions")) {
            animation.smoothTransitions = animJson["smooth_transitions"].get<bool>();
        }
    }
}

nlohmann::json RenderConfig::toJson() const {
    nlohmann::json json;
    
    // Layout
    switch (layout) {
        case LayoutType::Horizontal: json["layout"] = "horizontal"; break;
        case LayoutType::Vertical: json["layout"] = "vertical"; break;
        case LayoutType::Circular: json["layout"] = "circular"; break;
        case LayoutType::Grid: json["layout"] = "grid"; break;
        case LayoutType::ForceDirected: json["layout"] = "force_directed"; break;
    }
    
    json["network_position"] = {networkPosition.x, networkPosition.y};
    json["zoom"] = zoom;
    
    // Color scheme
    switch (colorScheme) {
        case ColorScheme::Default: json["color_scheme"] = "default"; break;
        case ColorScheme::BlueGradient: json["color_scheme"] = "blue_gradient"; break;
        case ColorScheme::RedGradient: json["color_scheme"] = "red_gradient"; break;
        case ColorScheme::Rainbow: json["color_scheme"] = "rainbow"; break;
        case ColorScheme::Grayscale: json["color_scheme"] = "grayscale"; break;
        case ColorScheme::HighContrast: json["color_scheme"] = "high_contrast"; break;
        case ColorScheme::Custom: json["color_scheme"] = "custom"; break;
    }
    
    // Neuron configuration
    json["neuron"]["radius"] = neuron.radius;
    json["neuron"]["show_value"] = neuron.showValue;
    json["neuron"]["show_bias"] = neuron.showBias;
    
    // Connection configuration
    json["connection"]["thickness"] = connection.thickness;
    json["connection"]["show_weight_values"] = connection.showWeightValues;
    json["connection"]["animate_flow"] = connection.animateFlow;
    
    // Layer configuration
    json["layer"]["spacing"] = layer.spacing;
    json["layer"]["neuron_spacing"] = layer.neuronSpacing;
    json["layer"]["show_label"] = layer.showLabel;
    
    // Animation configuration
    json["animation"]["enabled"] = animation.enabled;
    json["animation"]["duration"] = animation.duration;
    json["animation"]["smooth_transitions"] = animation.smoothTransitions;
    
    return json;
}

} // namespace graphics
} // namespace nnv
