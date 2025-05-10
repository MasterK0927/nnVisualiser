/**
 * @file ConfigManager.cpp
 * @brief Implementation of the configuration management system
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include "utils/ConfigManager.hpp"
#include "utils/Logger.hpp"
#include <fstream>
#include <iostream>

namespace nnv {
namespace utils {

ConfigManager::ConfigManager() {
    initializeDefaults();
}

bool ConfigManager::parseCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            std::cout << "Neural Network Visualizer v" << NNV_VERSION_STRING << "\n";
            std::cout << "Usage: " << argv[0] << " [options]\n";
            std::cout << "Options:\n";
            std::cout << "  --help, -h              Show this help message\n";
            std::cout << "  --config, -c <file>     Load configuration from file\n";
            std::cout << "  --network, -n <file>    Load neural network from file\n";
            std::cout << "  --width, -w <pixels>    Set window width\n";
            std::cout << "  --height, -h <pixels>   Set window height\n";
            std::cout << "  --fullscreen, -f        Start in fullscreen mode\n";
            std::cout << "  --log-level, -l <level> Set log level (debug, info, warning, error, critical)\n";
            std::cout << "  --log-file <file>       Set log file path\n";
            return false;
        }
        else if ((arg == "--config" || arg == "-c") && i + 1 < argc) {
            std::string configFile = argv[++i];
            if (!loadFromFile(configFile)) {
                std::cerr << "Failed to load configuration from: " << configFile << std::endl;
                return false;
            }
        }
        else if ((arg == "--network" || arg == "-n") && i + 1 < argc) {
            set("startup.network_file", std::string(argv[++i]));
        }
        else if ((arg == "--width" || arg == "-w") && i + 1 < argc) {
            try {
                int width = std::stoi(argv[++i]);
                set("window.width", width);
            } catch (const std::exception& e) {
                std::cerr << "Invalid width value: " << argv[i] << std::endl;
                return false;
            }
        }
        else if ((arg == "--height") && i + 1 < argc) {
            try {
                int height = std::stoi(argv[++i]);
                set("window.height", height);
            } catch (const std::exception& e) {
                std::cerr << "Invalid height value: " << argv[i] << std::endl;
                return false;
            }
        }
        else if (arg == "--fullscreen" || arg == "-f") {
            set("window.fullscreen", true);
        }
        else if ((arg == "--log-level" || arg == "-l") && i + 1 < argc) {
            set("logging.level", std::string(argv[++i]));
        }
        else if (arg == "--log-file" && i + 1 < argc) {
            set("logging.file", std::string(argv[++i]));
        }
        else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return false;
        }
    }
    
    return true;
}

bool ConfigManager::loadFromFile(const std::string& filename, ConfigFormat format) {
    try {
        if (format == ConfigFormat::Auto) {
            format = detectFormat(filename);
        }
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            NNV_LOG_ERROR("Failed to open configuration file: {}", filename);
            return false;
        }
        
        if (format == ConfigFormat::JSON) {
            file >> config_;
        }
#ifdef HAS_YAML_CPP
        else if (format == ConfigFormat::YAML) {
            YAML::Node yamlNode = YAML::LoadFile(filename);
            // Convert YAML to JSON (simplified conversion)
            std::ostringstream oss;
            oss << yamlNode;
            config_ = nlohmann::json::parse(oss.str());
        }
#endif
        else {
            NNV_LOG_ERROR("Unsupported configuration format for file: {}", filename);
            return false;
        }
        
        NNV_LOG_INFO("Configuration loaded from: {}", filename);
        return true;
        
    } catch (const std::exception& e) {
        NNV_LOG_ERROR("Failed to load configuration from {}: {}", filename, e.what());
        return false;
    }
}

bool ConfigManager::saveToFile(const std::string& filename, ConfigFormat format) const {
    try {
        if (format == ConfigFormat::Auto) {
            format = detectFormat(filename);
        }
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            NNV_LOG_ERROR("Failed to create configuration file: {}", filename);
            return false;
        }
        
        if (format == ConfigFormat::JSON) {
            file << config_.dump(4);
        }
#ifdef HAS_YAML_CPP
        else if (format == ConfigFormat::YAML) {
            // Convert JSON to YAML (simplified conversion)
            YAML::Node yamlNode = YAML::Load(config_.dump());
            file << yamlNode;
        }
#endif
        else {
            NNV_LOG_ERROR("Unsupported configuration format for file: {}", filename);
            return false;
        }
        
        NNV_LOG_INFO("Configuration saved to: {}", filename);
        return true;
        
    } catch (const std::exception& e) {
        NNV_LOG_ERROR("Failed to save configuration to {}: {}", filename, e.what());
        return false;
    }
}

core::NetworkConfig ConfigManager::loadNetworkConfig(const nlohmann::json& json) const {
    core::NetworkConfig config;
    
    if (json.contains("name")) {
        config.name = json["name"].get<std::string>();
    }
    
    if (json.contains("optimizer")) {
        config.optimizer = parseOptimizerType(json["optimizer"].get<std::string>());
    }
    
    if (json.contains("loss")) {
        config.loss = parseLossType(json["loss"].get<std::string>());
    }
    
    if (json.contains("layers") && json["layers"].is_array()) {
        for (const auto& layerJson : json["layers"]) {
            core::LayerConfig layerConfig;
            
            layerConfig.size = layerJson["size"].get<core::LayerSize>();
            
            if (layerJson.contains("activation")) {
                layerConfig.activation = parseActivationType(layerJson["activation"].get<std::string>());
            }
            
            if (layerJson.contains("dropout")) {
                layerConfig.dropout_rate = layerJson["dropout"].get<core::Scalar>();
            }
            
            if (layerJson.contains("name")) {
                layerConfig.name = layerJson["name"].get<std::string>();
            }
            
            config.layers.push_back(layerConfig);
        }
    }
    
    if (json.contains("training")) {
        const auto& trainingJson = json["training"];
        
        if (trainingJson.contains("learning_rate")) {
            config.training.learning_rate = trainingJson["learning_rate"].get<core::Scalar>();
        }
        
        if (trainingJson.contains("batch_size")) {
            config.training.batch_size = trainingJson["batch_size"].get<std::size_t>();
        }
        
        if (trainingJson.contains("epochs")) {
            config.training.epochs = trainingJson["epochs"].get<std::size_t>();
        }
    }
    
    return config;
}

nlohmann::json ConfigManager::saveNetworkConfig(const core::NetworkConfig& config) const {
    nlohmann::json json;
    
    json["name"] = config.name;
    json["optimizer"] = optimizerTypeToString(config.optimizer);
    json["loss"] = lossTypeToString(config.loss);
    
    json["layers"] = nlohmann::json::array();
    for (const auto& layer : config.layers) {
        nlohmann::json layerJson;
        layerJson["size"] = layer.size;
        layerJson["activation"] = activationTypeToString(layer.activation);
        layerJson["dropout"] = layer.dropout_rate;
        layerJson["name"] = layer.name;
        json["layers"].push_back(layerJson);
    }
    
    json["training"]["learning_rate"] = config.training.learning_rate;
    json["training"]["batch_size"] = config.training.batch_size;
    json["training"]["epochs"] = config.training.epochs;
    json["training"]["validation_split"] = config.training.validation_split;
    
    return json;
}

bool ConfigManager::has(const std::string& key) const {
    try {
        getValueFromPath<nlohmann::json>(config_, key);
        return true;
    } catch (...) {
        return false;
    }
}

void ConfigManager::reset() {
    initializeDefaults();
}

nlohmann::json ConfigManager::getDefaultConfig() {
    nlohmann::json config;
    
    // Window settings
    config["window"]["width"] = 1280;
    config["window"]["height"] = 720;
    config["window"]["fullscreen"] = false;
    config["window"]["vsync"] = true;
    config["window"]["title"] = "Neural Network Visualizer";
    
    // Rendering settings
    config["rendering"]["target_fps"] = 60;
    config["rendering"]["antialiasing"] = 4;
    
    // UI settings
    config["ui"]["theme"] = "dark";
    config["ui"]["font_size"] = 16;
    config["ui"]["docking_enabled"] = true;
    
    // Logging settings
    config["logging"]["level"] = "info";
    config["logging"]["file"] = "";
    config["logging"]["console"] = true;
    
    // Startup settings
    config["startup"]["network_file"] = "";
    config["startup"]["auto_load_last"] = false;
    
    return config;
}

void ConfigManager::initializeDefaults() {
    config_ = getDefaultConfig();
}

ConfigFormat ConfigManager::detectFormat(const std::string& filename) const {
    std::string ext = getFileExtension(filename);
    ext = toLower(ext);

    if (ext == ".json") {
        return ConfigFormat::JSON;
    } else if (ext == ".yaml" || ext == ".yml") {
        return ConfigFormat::YAML;
    }

    return ConfigFormat::JSON; // Default to JSON
}

core::ActivationType ConfigManager::parseActivationType(const std::string& str) const {
    std::string lower = toLower(str);

    if (lower == "none") return core::ActivationType::None;
    if (lower == "relu") return core::ActivationType::ReLU;
    if (lower == "sigmoid") return core::ActivationType::Sigmoid;
    if (lower == "tanh") return core::ActivationType::Tanh;
    if (lower == "leakyrelu") return core::ActivationType::LeakyReLU;
    if (lower == "elu") return core::ActivationType::ELU;
    if (lower == "swish") return core::ActivationType::Swish;
    if (lower == "gelu") return core::ActivationType::GELU;
    if (lower == "softmax") return core::ActivationType::Softmax;

    NNV_LOG_WARNING("Unknown activation type: {}, defaulting to ReLU", str);
    return core::ActivationType::ReLU;
}

std::string ConfigManager::activationTypeToString(core::ActivationType type) const {
    switch (type) {
        case core::ActivationType::None: return "none";
        case core::ActivationType::ReLU: return "relu";
        case core::ActivationType::Sigmoid: return "sigmoid";
        case core::ActivationType::Tanh: return "tanh";
        case core::ActivationType::LeakyReLU: return "leakyrelu";
        case core::ActivationType::ELU: return "elu";
        case core::ActivationType::Swish: return "swish";
        case core::ActivationType::GELU: return "gelu";
        case core::ActivationType::Softmax: return "softmax";
        default: return "relu";
    }
}

core::OptimizerType ConfigManager::parseOptimizerType(const std::string& str) const {
    std::string lower = toLower(str);

    if (lower == "sgd") return core::OptimizerType::SGD;
    if (lower == "adam") return core::OptimizerType::Adam;
    if (lower == "rmsprop") return core::OptimizerType::RMSprop;
    if (lower == "adagrad") return core::OptimizerType::AdaGrad;

    NNV_LOG_WARNING("Unknown optimizer type: {}, defaulting to Adam", str);
    return core::OptimizerType::Adam;
}

std::string ConfigManager::optimizerTypeToString(core::OptimizerType type) const {
    switch (type) {
        case core::OptimizerType::SGD: return "sgd";
        case core::OptimizerType::Adam: return "adam";
        case core::OptimizerType::RMSprop: return "rmsprop";
        case core::OptimizerType::AdaGrad: return "adagrad";
        default: return "adam";
    }
}

core::LossType ConfigManager::parseLossType(const std::string& str) const {
    std::string lower = toLower(str);

    if (lower == "mse" || lower == "meansquarederror") return core::LossType::MeanSquaredError;
    if (lower == "crossentropy") return core::LossType::CrossEntropy;
    if (lower == "binarycrossentropy") return core::LossType::BinaryCrossEntropy;
    if (lower == "huber") return core::LossType::Huber;
    if (lower == "focalloss") return core::LossType::FocalLoss;

    NNV_LOG_WARNING("Unknown loss type: {}, defaulting to MSE", str);
    return core::LossType::MeanSquaredError;
}

std::string ConfigManager::lossTypeToString(core::LossType type) const {
    switch (type) {
        case core::LossType::MeanSquaredError: return "mse";
        case core::LossType::CrossEntropy: return "crossentropy";
        case core::LossType::BinaryCrossEntropy: return "binarycrossentropy";
        case core::LossType::Huber: return "huber";
        case core::LossType::FocalLoss: return "focalloss";
        default: return "mse";
    }
}

} // namespace utils
} // namespace nnv
