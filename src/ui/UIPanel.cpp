/**
 * @file UIPanel.cpp
 * @brief Implementation of the UIPanel base class
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include "ui/UIPanel.hpp"
#include <sstream>
#include <random>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

namespace nnv {
namespace ui {

UIPanel::UIPanel(const std::string& title, const std::string& id)
    : title_(title)
    , id_(id.empty() ? generateId() : id)
    , visible_(true)
    , focused_(false)
    , flags_(0)
    , position_{100.0f, 100.0f}
    , size_{400.0f, 300.0f}
    , callback_(nullptr)
    , dockable_(true)
    , dockId_(nullptr)
{
#ifdef HAS_IMGUI
    // Set default ImGui window flags
    flags_ = ImGuiWindowFlags_None;
#endif
}

void UIPanel::update(float deltaTime) {
    // Base implementation does nothing
    // Derived classes can override this for custom update logic
    (void)deltaTime; // Suppress unused parameter warning
}

bool UIPanel::beginPanel() {
    if (!visible_) {
        return false;
    }
    
#ifdef HAS_IMGUI
    // Set window position and size if specified
    if (position_.first >= 0 && position_.second >= 0) {
        ImGui::SetNextWindowPos(ImVec2(position_.first, position_.second), ImGuiCond_FirstUseEver);
    }
    
    if (size_.first > 0 && size_.second > 0) {
        ImGui::SetNextWindowSize(ImVec2(size_.first, size_.second), ImGuiCond_FirstUseEver);
    }
    
    // Set dock ID if specified
    if (dockId_ && dockable_) {
        ImGui::SetNextWindowDockID(reinterpret_cast<ImGuiID>(dockId_), ImGuiCond_FirstUseEver);
    }
    
    // Begin window
    std::string windowTitle = title_ + "###" + id_;
    bool open = true;
    bool result = ImGui::Begin(windowTitle.c_str(), &open, flags_);
    
    // Update visibility based on close button
    if (!open) {
        visible_ = false;
    }
    
    // Update focus state
    focused_ = ImGui::IsWindowFocused();
    
    return result;
#else
    return true;
#endif
}

void UIPanel::endPanel() {
#ifdef HAS_IMGUI
    ImGui::End();
#endif
}

std::string UIPanel::generateId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1000, 9999);
    
    std::ostringstream oss;
    oss << "panel_" << dis(gen);
    return oss.str();
}

} // namespace ui
} // namespace nnv
