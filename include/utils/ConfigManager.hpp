/**
 * @file ConfigManager.hpp
 * @brief Configuration management system for the Neural Network Visualizer
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <nlohmann/json.hpp>

#ifdef HAS_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

#include "core/Types.hpp"
#include "utils/Common.hpp"

namespace nnv {
namespace utils {

/**
 * @brief Configuration file format types
 */
enum class ConfigFormat {
    JSON,
    YAML,
    Auto  // Detect from file extension
};

/**
 * @brief Main configuration management class
 */
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager() = default;
    
    /**
     * @brief Parse command line arguments
     * @param argc Number of arguments
     * @param argv Array of argument strings
     * @return True if parsing was successful
     */
    bool parseCommandLine(int argc, char* argv[]);
    
    /**
     * @brief Load configuration from file
     * @param filename Path to configuration file
     * @param format File format (auto-detected if not specified)
     * @return True if loading was successful
     */
    bool loadFromFile(const std::string& filename, ConfigFormat format = ConfigFormat::Auto);
    
    /**
     * @brief Save configuration to file
     * @param filename Path to configuration file
     * @param format File format (auto-detected if not specified)
     * @return True if saving was successful
     */
    bool saveToFile(const std::string& filename, ConfigFormat format = ConfigFormat::Auto) const;
    
    /**
     * @brief Load network configuration from JSON
     * @param json JSON object containing network configuration
     * @return Network configuration structure
     */
    core::NetworkConfig loadNetworkConfig(const nlohmann::json& json) const;
    
    /**
     * @brief Save network configuration to JSON
     * @param config Network configuration to save
     * @return JSON object containing the configuration
     */
    nlohmann::json saveNetworkConfig(const core::NetworkConfig& config) const;
    
    /**
     * @brief Get a configuration value
     * @tparam T Type of the value
     * @param key Configuration key (dot-separated path)
     * @param defaultValue Default value if key doesn't exist
     * @return Configuration value or default
     */
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T{}) const {
        try {
            return getValueFromPath<T>(config_, key);
        } catch (...) {
            return defaultValue;
        }
    }
    
    /**
     * @brief Set a configuration value
     * @tparam T Type of the value
     * @param key Configuration key (dot-separated path)
     * @param value Value to set
     */
    template<typename T>
    void set(const std::string& key, const T& value) {
        setValueAtPath(config_, key, value);
    }
    
    /**
     * @brief Check if a configuration key exists
     * @param key Configuration key to check
     * @return True if key exists
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Get the raw JSON configuration
     * @return Reference to the JSON configuration
     */
    const nlohmann::json& getJson() const { return config_; }
    
    /**
     * @brief Set the raw JSON configuration
     * @param json New JSON configuration
     */
    void setJson(const nlohmann::json& json) { config_ = json; }
    
    /**
     * @brief Reset configuration to defaults
     */
    void reset();
    
    /**
     * @brief Get default configuration
     * @return Default JSON configuration
     */
    static nlohmann::json getDefaultConfig();
    
    // Convenience getters for common configuration values
    std::string getLogFile() const { return get<std::string>("logging.file", ""); }
    std::string getLogLevel() const { return get<std::string>("logging.level", "info"); }
    int getWindowWidth() const { return get<int>("window.width", 1280); }
    int getWindowHeight() const { return get<int>("window.height", 720); }
    bool getWindowFullscreen() const { return get<bool>("window.fullscreen", false); }
    bool getWindowVSync() const { return get<bool>("window.vsync", true); }
    int getTargetFPS() const { return get<int>("rendering.target_fps", 60); }
    std::string getTheme() const { return get<std::string>("ui.theme", "dark"); }
    
private:
    nlohmann::json config_;
    
    /**
     * @brief Initialize default configuration
     */
    void initializeDefaults();
    
    /**
     * @brief Detect configuration format from file extension
     * @param filename File path
     * @return Detected format
     */
    ConfigFormat detectFormat(const std::string& filename) const;
    
    /**
     * @brief Parse activation type from string
     * @param str String representation
     * @return Activation type
     */
    core::ActivationType parseActivationType(const std::string& str) const;
    
    /**
     * @brief Convert activation type to string
     * @param type Activation type
     * @return String representation
     */
    std::string activationTypeToString(core::ActivationType type) const;
    
    /**
     * @brief Parse optimizer type from string
     * @param str String representation
     * @return Optimizer type
     */
    core::OptimizerType parseOptimizerType(const std::string& str) const;
    
    /**
     * @brief Convert optimizer type to string
     * @param type Optimizer type
     * @return String representation
     */
    std::string optimizerTypeToString(core::OptimizerType type) const;
    
    /**
     * @brief Parse loss type from string
     * @param str String representation
     * @return Loss type
     */
    core::LossType parseLossType(const std::string& str) const;
    
    /**
     * @brief Convert loss type to string
     * @param type Loss type
     * @return String representation
     */
    std::string lossTypeToString(core::LossType type) const;
    
    /**
     * @brief Get value from JSON path
     * @tparam T Type of the value
     * @param json JSON object
     * @param path Dot-separated path
     * @return Value at path
     */
    template<typename T>
    T getValueFromPath(const nlohmann::json& json, const std::string& path) const {
        auto keys = split(path, '.');
        const nlohmann::json* current = &json;
        
        for (const auto& key : keys) {
            if (!current->contains(key)) {
                throw std::runtime_error("Key not found: " + path);
            }
            current = &(*current)[key];
        }
        
        return current->get<T>();
    }
    
    /**
     * @brief Set value at JSON path
     * @tparam T Type of the value
     * @param json JSON object
     * @param path Dot-separated path
     * @param value Value to set
     */
    template<typename T>
    void setValueAtPath(nlohmann::json& json, const std::string& path, const T& value) {
        auto keys = split(path, '.');
        nlohmann::json* current = &json;
        
        for (size_t i = 0; i < keys.size() - 1; ++i) {
            if (!current->contains(keys[i])) {
                (*current)[keys[i]] = nlohmann::json::object();
            }
            current = &(*current)[keys[i]];
        }
        
        (*current)[keys.back()] = value;
    }
};

} // namespace utils
} // namespace nnv
