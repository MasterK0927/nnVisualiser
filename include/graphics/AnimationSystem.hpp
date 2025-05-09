/**
 * @file AnimationSystem.hpp
 * @brief Animation system for neural network visualization
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <SFML/Graphics.hpp>

#include "graphics/RenderConfig.hpp"
#include "utils/Common.hpp"

namespace nnv {
namespace graphics {

/**
 * @brief Animation easing functions
 */
namespace easing {

/**
 * @brief Linear easing (no easing)
 * @param t Time parameter (0-1)
 * @return Eased value (0-1)
 */
float linear(float t);

/**
 * @brief Ease in (slow start)
 * @param t Time parameter (0-1)
 * @return Eased value (0-1)
 */
float easeIn(float t);

/**
 * @brief Ease out (slow end)
 * @param t Time parameter (0-1)
 * @return Eased value (0-1)
 */
float easeOut(float t);

/**
 * @brief Ease in-out (slow start and end)
 * @param t Time parameter (0-1)
 * @return Eased value (0-1)
 */
float easeInOut(float t);

/**
 * @brief Bounce easing
 * @param t Time parameter (0-1)
 * @return Eased value (0-1)
 */
float bounce(float t);

/**
 * @brief Elastic easing
 * @param t Time parameter (0-1)
 * @return Eased value (0-1)
 */
float elastic(float t);

/**
 * @brief Get easing function by type
 * @param type Easing type
 * @return Easing function
 */
std::function<float(float)> getEasingFunction(EasingType type);

} // namespace easing

/**
 * @brief Base animation class
 */
class Animation {
public:
    /**
     * @brief Constructor
     * @param duration Animation duration in seconds
     * @param easing Easing function type
     */
    Animation(float duration, EasingType easing = EasingType::Linear);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~Animation() = default;
    
    /**
     * @brief Update animation
     * @param deltaTime Time since last update
     * @return True if animation is still running
     */
    bool update(float deltaTime);
    
    /**
     * @brief Check if animation is finished
     * @return True if finished
     */
    bool isFinished() const { return finished_; }
    
    /**
     * @brief Get current progress (0-1)
     * @return Animation progress
     */
    float getProgress() const { return progress_; }
    
    /**
     * @brief Get eased progress (0-1)
     * @return Eased animation progress
     */
    float getEasedProgress() const;
    
    /**
     * @brief Reset animation
     */
    void reset();
    
    /**
     * @brief Set animation duration
     * @param duration New duration in seconds
     */
    void setDuration(float duration) { duration_ = duration; }
    
    /**
     * @brief Get animation duration
     * @return Duration in seconds
     */
    float getDuration() const { return duration_; }

protected:
    /**
     * @brief Apply animation (override in derived classes)
     * @param easedProgress Eased progress value (0-1)
     */
    virtual void apply(float easedProgress) = 0;

private:
    float duration_;                    ///< Animation duration
    float currentTime_;                 ///< Current time
    float progress_;                    ///< Raw progress (0-1)
    bool finished_;                     ///< Finished flag
    std::function<float(float)> easingFunc_; ///< Easing function
};

/**
 * @brief Float value animation
 */
class FloatAnimation : public Animation {
public:
    /**
     * @brief Constructor
     * @param target Target value reference
     * @param startValue Starting value
     * @param endValue Ending value
     * @param duration Animation duration
     * @param easing Easing type
     */
    FloatAnimation(float& target, float startValue, float endValue, 
                   float duration, EasingType easing = EasingType::Linear);

protected:
    void apply(float easedProgress) override;

private:
    float& target_;         ///< Target value reference
    float startValue_;      ///< Starting value
    float endValue_;        ///< Ending value
};

/**
 * @brief Vector2f animation
 */
class Vector2fAnimation : public Animation {
public:
    /**
     * @brief Constructor
     * @param target Target vector reference
     * @param startValue Starting vector
     * @param endValue Ending vector
     * @param duration Animation duration
     * @param easing Easing type
     */
    Vector2fAnimation(sf::Vector2f& target, const sf::Vector2f& startValue, 
                      const sf::Vector2f& endValue, float duration, 
                      EasingType easing = EasingType::Linear);

protected:
    void apply(float easedProgress) override;

private:
    sf::Vector2f& target_;      ///< Target vector reference
    sf::Vector2f startValue_;   ///< Starting vector
    sf::Vector2f endValue_;     ///< Ending vector
};

/**
 * @brief Color animation
 */
class ColorAnimation : public Animation {
public:
    /**
     * @brief Constructor
     * @param target Target color reference
     * @param startValue Starting color
     * @param endValue Ending color
     * @param duration Animation duration
     * @param easing Easing type
     */
    ColorAnimation(sf::Color& target, const sf::Color& startValue, 
                   const sf::Color& endValue, float duration, 
                   EasingType easing = EasingType::Linear);

protected:
    void apply(float easedProgress) override;

private:
    sf::Color& target_;         ///< Target color reference
    sf::Color startValue_;      ///< Starting color
    sf::Color endValue_;        ///< Ending color
};

/**
 * @brief Animation group for synchronized animations
 */
class AnimationGroup {
public:
    /**
     * @brief Constructor
     */
    AnimationGroup() = default;
    
    /**
     * @brief Destructor
     */
    ~AnimationGroup() = default;
    
    /**
     * @brief Add animation to group
     * @param animation Animation to add
     */
    void addAnimation(std::unique_ptr<Animation> animation);
    
    /**
     * @brief Update all animations
     * @param deltaTime Time since last update
     * @return True if any animations are still running
     */
    bool update(float deltaTime);
    
    /**
     * @brief Check if all animations are finished
     * @return True if all finished
     */
    bool isFinished() const;
    
    /**
     * @brief Reset all animations
     */
    void reset();
    
    /**
     * @brief Clear all animations
     */
    void clear();
    
    /**
     * @brief Get number of animations
     * @return Animation count
     */
    std::size_t size() const { return animations_.size(); }

private:
    std::vector<std::unique_ptr<Animation>> animations_; ///< Animation list
};

/**
 * @brief Main animation system
 */
class AnimationSystem {
public:
    /**
     * @brief Constructor
     * @param config Animation configuration
     */
    explicit AnimationSystem(const AnimationConfig& config);
    
    /**
     * @brief Destructor
     */
    ~AnimationSystem() = default;
    
    // Disable copy and move
    NNV_DISABLE_COPY_AND_MOVE(AnimationSystem)
    
    /**
     * @brief Update all animations
     * @param deltaTime Time since last update
     */
    void update(float deltaTime);
    
    /**
     * @brief Create float animation
     * @param target Target value reference
     * @param endValue Target end value
     * @param duration Animation duration (uses config default if 0)
     * @param easing Easing type (uses config default if not specified)
     * @return Animation ID for management
     */
    int animateFloat(float& target, float endValue, 
                     float duration = 0.0f, 
                     EasingType easing = EasingType::Linear);
    
    /**
     * @brief Create vector animation
     * @param target Target vector reference
     * @param endValue Target end value
     * @param duration Animation duration (uses config default if 0)
     * @param easing Easing type (uses config default if not specified)
     * @return Animation ID for management
     */
    int animateVector2f(sf::Vector2f& target, const sf::Vector2f& endValue,
                        float duration = 0.0f,
                        EasingType easing = EasingType::Linear);
    
    /**
     * @brief Create color animation
     * @param target Target color reference
     * @param endValue Target end value
     * @param duration Animation duration (uses config default if 0)
     * @param easing Easing type (uses config default if not specified)
     * @return Animation ID for management
     */
    int animateColor(sf::Color& target, const sf::Color& endValue,
                     float duration = 0.0f,
                     EasingType easing = EasingType::Linear);
    
    /**
     * @brief Stop animation by ID
     * @param animationId Animation ID
     */
    void stopAnimation(int animationId);
    
    /**
     * @brief Stop all animations
     */
    void stopAllAnimations();
    
    /**
     * @brief Check if animation is running
     * @param animationId Animation ID
     * @return True if running
     */
    bool isAnimationRunning(int animationId) const;
    
    /**
     * @brief Get number of active animations
     * @return Active animation count
     */
    std::size_t getActiveAnimationCount() const;
    
    /**
     * @brief Set animation configuration
     * @param config New animation configuration
     */
    void setConfig(const AnimationConfig& config) { config_ = config; }
    
    /**
     * @brief Get animation configuration
     * @return Current animation configuration
     */
    const AnimationConfig& getConfig() const { return config_; }
    
    /**
     * @brief Enable/disable animations
     * @param enabled Animation state
     */
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    /**
     * @brief Check if animations are enabled
     * @return True if enabled
     */
    bool isEnabled() const { return enabled_; }

private:
    AnimationConfig config_;                                    ///< Animation configuration
    bool enabled_;                                              ///< Animation enabled flag
    int nextAnimationId_;                                       ///< Next animation ID
    std::unordered_map<int, std::unique_ptr<Animation>> animations_; ///< Active animations
    
    /**
     * @brief Clean up finished animations
     */
    void cleanupFinishedAnimations();
    
    /**
     * @brief Get effective duration
     * @param requestedDuration Requested duration (0 for default)
     * @return Effective duration
     */
    float getEffectiveDuration(float requestedDuration) const;
    
    /**
     * @brief Get effective easing
     * @param requestedEasing Requested easing
     * @return Effective easing
     */
    EasingType getEffectiveEasing(EasingType requestedEasing) const;
};

} // namespace graphics
} // namespace nnv
