/**
 * @file NetworkPanel.hpp
 * @brief Network information and editing panel
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <memory>
#include <vector>
#include <string>

#include "core/Types.hpp"
#include "ui/UIPanel.hpp"
#include "utils/Common.hpp"

namespace nnv {
namespace core {
    template<typename T> class NeuralNetwork;
}
}

namespace nnv {
namespace ui {

/**
 * @brief Network editing mode
 */
enum class NetworkEditMode {
    View,           ///< View only mode
    Edit,           ///< Edit structure mode
    Create          ///< Create new network mode
};

/**
 * @brief Layer editor data
 */
struct LayerEditor {
    core::LayerSize size = 1;
    core::ActivationType activation = core::ActivationType::ReLU;
    std::string name;
    float dropoutRate = 0.0f;
    bool trainable = true;
    bool markedForDeletion = false;
};

/**
 * @brief Network panel class for displaying and editing neural networks
 */
class NetworkPanel : public UIPanel {
public:
    /**
     * @brief Constructor
     */
    NetworkPanel();
    
    /**
     * @brief Destructor
     */
    ~NetworkPanel() override = default;
    
    /**
     * @brief Render the panel
     */
    void render() override;
    
    /**
     * @brief Set neural network
     * @param network Neural network to display/edit
     */
    void setNeuralNetwork(std::shared_ptr<core::DefaultNetwork> network);
    
    /**
     * @brief Get neural network
     * @return Current neural network
     */
    std::shared_ptr<core::DefaultNetwork> getNeuralNetwork() const { return network_; }
    
    /**
     * @brief Set edit mode
     * @param mode Edit mode
     */
    void setEditMode(NetworkEditMode mode) { editMode_ = mode; }
    
    /**
     * @brief Get edit mode
     * @return Current edit mode
     */
    NetworkEditMode getEditMode() const { return editMode_; }
    
    /**
     * @brief Check if network has been modified
     * @return True if modified
     */
    bool isModified() const { return modified_; }
    
    /**
     * @brief Apply changes to network
     * @return True if successful
     */
    bool applyChanges();
    
    /**
     * @brief Discard changes
     */
    void discardChanges();
    
    /**
     * @brief Create new network from editor
     * @return New neural network
     */
    std::shared_ptr<core::DefaultNetwork> createNetwork();

private:
    std::shared_ptr<core::DefaultNetwork> network_;     ///< Current neural network
    NetworkEditMode editMode_;                          ///< Current edit mode
    bool modified_;                                     ///< Modification flag
    
    // Editor state
    std::vector<LayerEditor> layerEditors_;             ///< Layer editors
    std::string networkName_;                           ///< Network name
    float learningRate_;                                ///< Learning rate
    core::LossType lossType_;                          ///< Loss function type
    core::OptimizerType optimizerType_;                ///< Optimizer type
    
    // UI state
    int selectedLayer_;                                 ///< Selected layer index
    bool showAdvanced_;                                 ///< Show advanced options
    
    /**
     * @brief Render network information
     */
    void renderNetworkInfo();
    
    /**
     * @brief Render layer list
     */
    void renderLayerList();
    
    /**
     * @brief Render layer editor
     * @param index Layer index
     */
    void renderLayerEditor(std::size_t index);
    
    /**
     * @brief Render network settings
     */
    void renderNetworkSettings();
    
    /**
     * @brief Render control buttons
     */
    void renderControls();
    
    /**
     * @brief Add new layer
     */
    void addLayer();
    
    /**
     * @brief Remove layer
     * @param index Layer index
     */
    void removeLayer(std::size_t index);
    
    /**
     * @brief Move layer up
     * @param index Layer index
     */
    void moveLayerUp(std::size_t index);
    
    /**
     * @brief Move layer down
     * @param index Layer index
     */
    void moveLayerDown(std::size_t index);
    
    /**
     * @brief Initialize editor from network
     */
    void initializeFromNetwork();
    
    /**
     * @brief Validate network configuration
     * @return True if valid
     */
    bool validateConfiguration() const;
    
    /**
     * @brief Get activation type name
     * @param type Activation type
     * @return Human-readable name
     */
    const char* getActivationTypeName(core::ActivationType type) const;
    
    /**
     * @brief Get loss type name
     * @param type Loss type
     * @return Human-readable name
     */
    const char* getLossTypeName(core::LossType type) const;
    
    /**
     * @brief Get optimizer type name
     * @param type Optimizer type
     * @return Human-readable name
     */
    const char* getOptimizerTypeName(core::OptimizerType type) const;
    
    /**
     * @brief Render activation type combo
     * @param type Current activation type
     * @return True if changed
     */
    bool renderActivationTypeCombo(core::ActivationType& type);
    
    /**
     * @brief Render loss type combo
     * @param type Current loss type
     * @return True if changed
     */
    bool renderLossTypeCombo(core::LossType& type);
    
    /**
     * @brief Render optimizer type combo
     * @param type Current optimizer type
     * @return True if changed
     */
    bool renderOptimizerTypeCombo(core::OptimizerType& type);
};

} // namespace ui
} // namespace nnv
