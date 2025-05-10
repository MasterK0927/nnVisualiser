/**
 * @file NetworkPanel.cpp
 * @brief Implementation of the NetworkPanel class
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include "ui/NetworkPanel.hpp"
#include "core/NeuralNetwork.hpp"
#include "utils/Logger.hpp"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

namespace nnv {
namespace ui {

NetworkPanel::NetworkPanel()
    : UIPanel("Network", "network_panel")
    , network_(nullptr)
    , editMode_(NetworkEditMode::View)
    , modified_(false)
    , networkName_("New Network")
    , learningRate_(0.001f)
    , lossType_(core::LossType::MeanSquaredError)
    , optimizerType_(core::OptimizerType::Adam)
    , selectedLayer_(-1)
    , showAdvanced_(false)
{
}

void NetworkPanel::render() {
    if (!beginPanel()) {
        return;
    }
    
#ifdef HAS_IMGUI
    // Render network information
    renderNetworkInfo();
    
    ImGui::Separator();
    
    // Render layer list
    renderLayerList();
    
    ImGui::Separator();
    
    // Render network settings
    if (editMode_ != NetworkEditMode::View) {
        renderNetworkSettings();
        ImGui::Separator();
    }
    
    // Render controls
    renderControls();
#endif
    
    endPanel();
}

void NetworkPanel::setNeuralNetwork(std::shared_ptr<core::DefaultNetwork> network) {
    network_ = network;
    initializeFromNetwork();
    modified_ = false;
}

bool NetworkPanel::applyChanges() {
    if (!network_ || !validateConfiguration()) {
        return false;
    }
    
    try {
        // Apply network-level changes
        network_->setName(networkName_);
        network_->setLearningRate(learningRate_);
        network_->setLossType(lossType_);
        network_->setOptimizerType(optimizerType_);
        
        // Note: Layer structure changes would require rebuilding the network
        // This is a simplified implementation
        
        modified_ = false;
        NNV_LOG_INFO("Applied changes to network: {}", networkName_);
        return true;
        
    } catch (const std::exception& e) {
        NNV_LOG_ERROR("Failed to apply network changes: {}", e.what());
        return false;
    }
}

void NetworkPanel::discardChanges() {
    initializeFromNetwork();
    modified_ = false;
}

std::shared_ptr<core::DefaultNetwork> NetworkPanel::createNetwork() {
    if (!validateConfiguration()) {
        return nullptr;
    }
    
    try {
        core::NetworkConfig config;
        config.name = networkName_;
        config.training.learning_rate = learningRate_;
        config.loss = lossType_;
        config.optimizer = optimizerType_;
        
        // Convert layer editors to layer configs
        for (const auto& editor : layerEditors_) {
            core::LayerConfig layerConfig;
            layerConfig.size = editor.size;
            layerConfig.activation = editor.activation;
            layerConfig.name = editor.name;
            layerConfig.dropout_rate = editor.dropoutRate;
            layerConfig.trainable = editor.trainable;
            config.layers.push_back(layerConfig);
        }
        
        auto network = std::make_shared<core::DefaultNetwork>(config);
        NNV_LOG_INFO("Created new network: {}", networkName_);
        return network;
        
    } catch (const std::exception& e) {
        NNV_LOG_ERROR("Failed to create network: {}", e.what());
        return nullptr;
    }
}

void NetworkPanel::renderNetworkInfo() {
#ifdef HAS_IMGUI
    ImGui::Text("Network Information");
    
    if (network_) {
        ImGui::Text("Name: %s", network_->getName().c_str());
        ImGui::Text("Layers: %zu", network_->getLayerCount());
        ImGui::Text("Learning Rate: %.6f", network_->getLearningRate());
        ImGui::Text("Loss Function: %s", getLossTypeName(network_->getLossType()));
        ImGui::Text("Optimizer: %s", getOptimizerTypeName(network_->getOptimizerType()));
        
        if (network_->isTraining()) {
            ImGui::Text("Status: Training (%.1f%%)", network_->getTrainingProgress() * 100.0f);
        } else {
            ImGui::Text("Status: Ready");
        }
    } else {
        ImGui::Text("No network loaded");
    }
#endif
}

void NetworkPanel::renderLayerList() {
#ifdef HAS_IMGUI
    ImGui::Text("Network Architecture");
    
    if (editMode_ != NetworkEditMode::View) {
        if (ImGui::Button("Add Layer")) {
            addLayer();
        }
        ImGui::SameLine();
        if (ImGui::Button("Remove Selected") && selectedLayer_ >= 0) {
            removeLayer(selectedLayer_);
        }
    }
    
    // Display layers
    if (network_ || !layerEditors_.empty()) {
        ImGui::BeginChild("LayerList", ImVec2(0, 200), true);
        
        if (editMode_ == NetworkEditMode::View && network_) {
            // View mode - show actual network layers
            for (std::size_t i = 0; i < network_->getLayerCount(); ++i) {
                const auto& layer = network_->getLayer(i);
                
                bool selected = (static_cast<int>(i) == selectedLayer_);
                if (ImGui::Selectable(("Layer " + std::to_string(i) + ": " + layer.getName()).c_str(), selected)) {
                    selectedLayer_ = static_cast<int>(i);
                }
                
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("Size: %zu", layer.getSize());
                    ImGui::Text("Activation: %s", getActivationTypeName(layer.getActivationType()));
                    ImGui::Text("Dropout: %.2f", layer.getDropoutRate());
                    ImGui::EndTooltip();
                }
            }
        } else {
            // Edit mode - show layer editors
            for (std::size_t i = 0; i < layerEditors_.size(); ++i) {
                if (layerEditors_[i].markedForDeletion) continue;
                
                bool selected = (static_cast<int>(i) == selectedLayer_);
                std::string label = "Layer " + std::to_string(i) + ": " + layerEditors_[i].name;
                
                if (ImGui::Selectable(label.c_str(), selected)) {
                    selectedLayer_ = static_cast<int>(i);
                }
                
                if (selected) {
                    renderLayerEditor(i);
                }
            }
        }
        
        ImGui::EndChild();
    }
#endif
}

void NetworkPanel::renderLayerEditor(std::size_t index) {
#ifdef HAS_IMGUI
    if (index >= layerEditors_.size()) return;
    
    auto& editor = layerEditors_[index];
    
    ImGui::Separator();
    ImGui::Text("Edit Layer %zu", index);
    
    // Layer name
    char nameBuffer[256];
    strncpy(nameBuffer, editor.name.c_str(), sizeof(nameBuffer) - 1);
    nameBuffer[sizeof(nameBuffer) - 1] = '\0';
    
    if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
        editor.name = nameBuffer;
        modified_ = true;
    }
    
    // Layer size
    int size = static_cast<int>(editor.size);
    if (ImGui::InputInt("Size", &size, 1, 10)) {
        if (size > 0) {
            editor.size = static_cast<core::LayerSize>(size);
            modified_ = true;
        }
    }
    
    // Activation function
    if (renderActivationTypeCombo(editor.activation)) {
        modified_ = true;
    }
    
    // Dropout rate
    if (ImGui::SliderFloat("Dropout", &editor.dropoutRate, 0.0f, 0.9f, "%.2f")) {
        modified_ = true;
    }
    
    // Trainable flag
    if (ImGui::Checkbox("Trainable", &editor.trainable)) {
        modified_ = true;
    }
#endif
}

void NetworkPanel::renderNetworkSettings() {
#ifdef HAS_IMGUI
    ImGui::Text("Network Settings");
    
    // Network name
    char nameBuffer[256];
    strncpy(nameBuffer, networkName_.c_str(), sizeof(nameBuffer) - 1);
    nameBuffer[sizeof(nameBuffer) - 1] = '\0';
    
    if (ImGui::InputText("Network Name", nameBuffer, sizeof(nameBuffer))) {
        networkName_ = nameBuffer;
        modified_ = true;
    }
    
    // Learning rate
    if (ImGui::InputFloat("Learning Rate", &learningRate_, 0.0001f, 0.01f, "%.6f")) {
        if (learningRate_ > 0.0f) {
            modified_ = true;
        } else {
            learningRate_ = 0.001f;
        }
    }
    
    // Loss function
    if (renderLossTypeCombo(lossType_)) {
        modified_ = true;
    }
    
    // Optimizer
    if (renderOptimizerTypeCombo(optimizerType_)) {
        modified_ = true;
    }
#endif
}

void NetworkPanel::renderControls() {
#ifdef HAS_IMGUI
    ImGui::Text("Controls");
    
    // Edit mode buttons
    if (editMode_ == NetworkEditMode::View) {
        if (ImGui::Button("Edit Network") && network_) {
            setEditMode(NetworkEditMode::Edit);
            initializeFromNetwork();
        }
        ImGui::SameLine();
        if (ImGui::Button("Create New")) {
            setEditMode(NetworkEditMode::Create);
            layerEditors_.clear();
            addLayer(); // Start with one layer
        }
    } else {
        if (ImGui::Button("Apply Changes")) {
            if (editMode_ == NetworkEditMode::Edit) {
                applyChanges();
            } else if (editMode_ == NetworkEditMode::Create) {
                auto newNetwork = createNetwork();
                if (newNetwork) {
                    setNeuralNetwork(newNetwork);
                    setEditMode(NetworkEditMode::View);
                    executeCallback(); // Notify parent
                }
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            if (editMode_ == NetworkEditMode::Edit) {
                discardChanges();
            }
            setEditMode(NetworkEditMode::View);
        }
        
        if (modified_) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Modified");
        }
    }
#endif
}

void NetworkPanel::addLayer() {
    LayerEditor editor;
    editor.name = "Layer " + std::to_string(layerEditors_.size());
    editor.size = 10;
    editor.activation = core::ActivationType::ReLU;
    
    layerEditors_.push_back(editor);
    selectedLayer_ = static_cast<int>(layerEditors_.size() - 1);
    modified_ = true;
}

void NetworkPanel::removeLayer(std::size_t index) {
    if (index < layerEditors_.size()) {
        layerEditors_.erase(layerEditors_.begin() + index);
        if (selectedLayer_ >= static_cast<int>(layerEditors_.size())) {
            selectedLayer_ = static_cast<int>(layerEditors_.size()) - 1;
        }
        modified_ = true;
    }
}

void NetworkPanel::initializeFromNetwork() {
    layerEditors_.clear();
    
    if (network_) {
        networkName_ = network_->getName();
        learningRate_ = network_->getLearningRate();
        lossType_ = network_->getLossType();
        optimizerType_ = network_->getOptimizerType();
        
        // Initialize layer editors from network layers
        for (std::size_t i = 0; i < network_->getLayerCount(); ++i) {
            const auto& layer = network_->getLayer(i);
            
            LayerEditor editor;
            editor.size = layer.getSize();
            editor.activation = layer.getActivationType();
            editor.name = layer.getName();
            editor.dropoutRate = layer.getDropoutRate();
            editor.trainable = layer.isTrainable();
            
            layerEditors_.push_back(editor);
        }
    }
    
    selectedLayer_ = -1;
}

bool NetworkPanel::validateConfiguration() const {
    if (layerEditors_.empty()) {
        return false;
    }
    
    // Check that all layers have valid sizes
    for (const auto& editor : layerEditors_) {
        if (editor.size == 0 || editor.markedForDeletion) {
            return false;
        }
    }
    
    return true;
}

const char* NetworkPanel::getActivationTypeName(core::ActivationType type) const {
    switch (type) {
        case core::ActivationType::None: return "None";
        case core::ActivationType::ReLU: return "ReLU";
        case core::ActivationType::Sigmoid: return "Sigmoid";
        case core::ActivationType::Tanh: return "Tanh";
        case core::ActivationType::LeakyReLU: return "Leaky ReLU";
        case core::ActivationType::ELU: return "ELU";
        case core::ActivationType::Swish: return "Swish";
        case core::ActivationType::GELU: return "GELU";
        case core::ActivationType::Softmax: return "Softmax";
        default: return "Unknown";
    }
}

const char* NetworkPanel::getLossTypeName(core::LossType type) const {
    switch (type) {
        case core::LossType::MeanSquaredError: return "Mean Squared Error";
        case core::LossType::CrossEntropy: return "Cross Entropy";
        case core::LossType::BinaryCrossEntropy: return "Binary Cross Entropy";
        case core::LossType::Huber: return "Huber";
        case core::LossType::FocalLoss: return "Focal Loss";
        default: return "Unknown";
    }
}

const char* NetworkPanel::getOptimizerTypeName(core::OptimizerType type) const {
    switch (type) {
        case core::OptimizerType::SGD: return "SGD";
        case core::OptimizerType::Adam: return "Adam";
        case core::OptimizerType::RMSprop: return "RMSprop";
        case core::OptimizerType::AdaGrad: return "AdaGrad";
        default: return "Unknown";
    }
}

bool NetworkPanel::renderActivationTypeCombo(core::ActivationType& type) {
#ifdef HAS_IMGUI
    const char* currentName = getActivationTypeName(type);
    bool changed = false;
    
    if (ImGui::BeginCombo("Activation", currentName)) {
        for (int i = 0; i < static_cast<int>(core::ActivationType::Softmax) + 1; ++i) {
            core::ActivationType testType = static_cast<core::ActivationType>(i);
            const char* name = getActivationTypeName(testType);
            
            bool selected = (type == testType);
            if (ImGui::Selectable(name, selected)) {
                type = testType;
                changed = true;
            }
            
            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    
    return changed;
#else
    return false;
#endif
}

bool NetworkPanel::renderLossTypeCombo(core::LossType& type) {
#ifdef HAS_IMGUI
    const char* currentName = getLossTypeName(type);
    bool changed = false;
    
    if (ImGui::BeginCombo("Loss Function", currentName)) {
        for (int i = 0; i < static_cast<int>(core::LossType::FocalLoss) + 1; ++i) {
            core::LossType testType = static_cast<core::LossType>(i);
            const char* name = getLossTypeName(testType);
            
            bool selected = (type == testType);
            if (ImGui::Selectable(name, selected)) {
                type = testType;
                changed = true;
            }
            
            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    
    return changed;
#else
    return false;
#endif
}

bool NetworkPanel::renderOptimizerTypeCombo(core::OptimizerType& type) {
#ifdef HAS_IMGUI
    const char* currentName = getOptimizerTypeName(type);
    bool changed = false;
    
    if (ImGui::BeginCombo("Optimizer", currentName)) {
        for (int i = 0; i < static_cast<int>(core::OptimizerType::AdaGrad) + 1; ++i) {
            core::OptimizerType testType = static_cast<core::OptimizerType>(i);
            const char* name = getOptimizerTypeName(testType);
            
            bool selected = (type == testType);
            if (ImGui::Selectable(name, selected)) {
                type = testType;
                changed = true;
            }
            
            if (selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    
    return changed;
#else
    return false;
#endif
}

} // namespace ui
} // namespace nnv
