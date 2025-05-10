/**
 * @file UIPanel.hpp
 * @brief Base class for UI panels
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <functional>
#include <vector>

#include "utils/Common.hpp"

namespace nnv {
namespace ui {

/**
 * @brief Base class for UI panels
 */
class UIPanel {
public:
    /**
     * @brief Constructor
     * @param title Panel title
     * @param id Panel ID
     */
    UIPanel(const std::string& title = "Panel", const std::string& id = "");
    
    /**
     * @brief Virtual destructor
     */
    virtual ~UIPanel() = default;
    
    // Disable copy and move
    NNV_DISABLE_COPY_AND_MOVE(UIPanel)
    
    /**
     * @brief Render the panel
     */
    virtual void render() = 0;
    
    /**
     * @brief Update the panel
     * @param deltaTime Time since last update
     */
    virtual void update(float deltaTime);
    
    /**
     * @brief Get panel title
     * @return Panel title
     */
    const std::string& getTitle() const { return title_; }
    
    /**
     * @brief Set panel title
     * @param title New panel title
     */
    void setTitle(const std::string& title) { title_ = title; }
    
    /**
     * @brief Get panel ID
     * @return Panel ID
     */
    const std::string& getId() const { return id_; }
    
    /**
     * @brief Check if panel is visible
     * @return True if visible
     */
    bool isVisible() const { return visible_; }
    
    /**
     * @brief Set panel visibility
     * @param visible Visibility state
     */
    void setVisible(bool visible) { visible_ = visible; }
    
    /**
     * @brief Toggle panel visibility
     * @return New visibility state
     */
    bool toggleVisibility() { visible_ = !visible_; return visible_; }
    
    /**
     * @brief Check if panel is focused
     * @return True if focused
     */
    bool isFocused() const { return focused_; }
    
    /**
     * @brief Set panel focus
     * @param focused Focus state
     */
    void setFocused(bool focused) { focused_ = focused; }
    
    /**
     * @brief Get panel flags
     * @return ImGui window flags
     */
    int getFlags() const { return flags_; }
    
    /**
     * @brief Set panel flags
     * @param flags ImGui window flags
     */
    void setFlags(int flags) { flags_ = flags; }
    
    /**
     * @brief Add panel flag
     * @param flag ImGui window flag
     */
    void addFlag(int flag) { flags_ |= flag; }
    
    /**
     * @brief Remove panel flag
     * @param flag ImGui window flag
     */
    void removeFlag(int flag) { flags_ &= ~flag; }
    
    /**
     * @brief Set panel position
     * @param x X position
     * @param y Y position
     */
    void setPosition(float x, float y) { position_ = {x, y}; }
    
    /**
     * @brief Get panel position
     * @return Panel position
     */
    const std::pair<float, float>& getPosition() const { return position_; }
    
    /**
     * @brief Set panel size
     * @param width Width
     * @param height Height
     */
    void setSize(float width, float height) { size_ = {width, height}; }
    
    /**
     * @brief Get panel size
     * @return Panel size
     */
    const std::pair<float, float>& getSize() const { return size_; }
    
    /**
     * @brief Set panel callback
     * @param callback Callback function
     */
    using PanelCallback = std::function<void()>;
    void setCallback(PanelCallback callback) { callback_ = callback; }
    
    /**
     * @brief Execute panel callback
     */
    void executeCallback() { if (callback_) callback_(); }
    
    /**
     * @brief Begin panel rendering
     * @return True if panel is open and should render content
     */
    bool beginPanel();
    
    /**
     * @brief End panel rendering
     */
    void endPanel();
    
    /**
     * @brief Set panel as dockable
     * @param dockable Dockable state
     */
    void setDockable(bool dockable) { dockable_ = dockable; }
    
    /**
     * @brief Check if panel is dockable
     * @return True if dockable
     */
    bool isDockable() const { return dockable_; }
    
    /**
     * @brief Set dock ID
     * @param dockId Dock space ID
     */
    void setDockId(void* dockId) { dockId_ = dockId; }
    
    /**
     * @brief Get dock ID
     * @return Dock space ID
     */
    void* getDockId() const { return dockId_; }

protected:
    std::string title_;                 ///< Panel title
    std::string id_;                    ///< Panel ID
    bool visible_;                      ///< Visibility state
    bool focused_;                      ///< Focus state
    int flags_;                         ///< ImGui window flags
    std::pair<float, float> position_;  ///< Panel position
    std::pair<float, float> size_;      ///< Panel size
    PanelCallback callback_;            ///< Panel callback
    bool dockable_;                     ///< Dockable state
    void* dockId_;                      ///< Dock space ID
    
    /**
     * @brief Generate unique ID
     * @return Unique ID string
     */
    static std::string generateId();
};

} // namespace ui
} // namespace nnv
