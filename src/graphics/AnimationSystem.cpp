/**
 * @file AnimationSystem.cpp
 * @brief Implementation of the animation system
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include "graphics/AnimationSystem.hpp"
#include "graphics/ColorScheme.hpp"
#include <cmath>
#include <algorithm>

namespace nnv {
namespace graphics {

namespace easing {

float linear(float t) {
    return t;
}

float easeIn(float t) {
    return t * t;
}

float easeOut(float t) {
    return 1.0f - (1.0f - t) * (1.0f - t);
}

float easeInOut(float t) {
    if (t < 0.5f) {
        return 2.0f * t * t;
    } else {
        return 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
    }
}

float bounce(float t) {
    if (t < 1.0f / 2.75f) {
        return 7.5625f * t * t;
    } else if (t < 2.0f / 2.75f) {
        t -= 1.5f / 2.75f;
        return 7.5625f * t * t + 0.75f;
    } else if (t < 2.5f / 2.75f) {
        t -= 2.25f / 2.75f;
        return 7.5625f * t * t + 0.9375f;
    } else {
        t -= 2.625f / 2.75f;
        return 7.5625f * t * t + 0.984375f;
    }
}

float elastic(float t) {
    if (t == 0.0f || t == 1.0f) {
        return t;
    }
    
    const float p = 0.3f;
    const float s = p / 4.0f;
    
    return std::pow(2.0f, -10.0f * t) * std::sin((t - s) * (2.0f * M_PI) / p) + 1.0f;
}

std::function<float(float)> getEasingFunction(EasingType type) {
    switch (type) {
        case EasingType::Linear:
            return linear;
        case EasingType::EaseIn:
            return easeIn;
        case EasingType::EaseOut:
            return easeOut;
        case EasingType::EaseInOut:
            return easeInOut;
        case EasingType::Bounce:
            return bounce;
        case EasingType::Elastic:
            return elastic;
        default:
            return linear;
    }
}

} // namespace easing

// Animation base class
Animation::Animation(float duration, EasingType easing)
    : duration_(duration)
    , currentTime_(0.0f)
    , progress_(0.0f)
    , finished_(false)
    , easingFunc_(easing::getEasingFunction(easing))
{
}

bool Animation::update(float deltaTime) {
    if (finished_) {
        return false;
    }
    
    currentTime_ += deltaTime;
    progress_ = std::min(currentTime_ / duration_, 1.0f);
    
    apply(getEasedProgress());
    
    if (progress_ >= 1.0f) {
        finished_ = true;
    }
    
    return !finished_;
}

float Animation::getEasedProgress() const {
    return easingFunc_(progress_);
}

void Animation::reset() {
    currentTime_ = 0.0f;
    progress_ = 0.0f;
    finished_ = false;
}

// FloatAnimation
FloatAnimation::FloatAnimation(float& target, float startValue, float endValue, 
                               float duration, EasingType easing)
    : Animation(duration, easing)
    , target_(target)
    , startValue_(startValue)
    , endValue_(endValue)
{
    target_ = startValue_;
}

void FloatAnimation::apply(float easedProgress) {
    target_ = startValue_ + (endValue_ - startValue_) * easedProgress;
}

// Vector2fAnimation
Vector2fAnimation::Vector2fAnimation(sf::Vector2f& target, const sf::Vector2f& startValue, 
                                     const sf::Vector2f& endValue, float duration, 
                                     EasingType easing)
    : Animation(duration, easing)
    , target_(target)
    , startValue_(startValue)
    , endValue_(endValue)
{
    target_ = startValue_;
}

void Vector2fAnimation::apply(float easedProgress) {
    target_.x = startValue_.x + (endValue_.x - startValue_.x) * easedProgress;
    target_.y = startValue_.y + (endValue_.y - startValue_.y) * easedProgress;
}

// ColorAnimation
ColorAnimation::ColorAnimation(sf::Color& target, const sf::Color& startValue, 
                               const sf::Color& endValue, float duration, 
                               EasingType easing)
    : Animation(duration, easing)
    , target_(target)
    , startValue_(startValue)
    , endValue_(endValue)
{
    target_ = startValue_;
}

void ColorAnimation::apply(float easedProgress) {
    target_ = color::lerp(startValue_, endValue_, easedProgress);
}

// AnimationGroup
void AnimationGroup::addAnimation(std::unique_ptr<Animation> animation) {
    if (animation) {
        animations_.push_back(std::move(animation));
    }
}

bool AnimationGroup::update(float deltaTime) {
    bool anyRunning = false;
    
    for (auto& animation : animations_) {
        if (animation->update(deltaTime)) {
            anyRunning = true;
        }
    }
    
    return anyRunning;
}

bool AnimationGroup::isFinished() const {
    for (const auto& animation : animations_) {
        if (!animation->isFinished()) {
            return false;
        }
    }
    return true;
}

void AnimationGroup::reset() {
    for (auto& animation : animations_) {
        animation->reset();
    }
}

void AnimationGroup::clear() {
    animations_.clear();
}

// AnimationSystem
AnimationSystem::AnimationSystem(const AnimationConfig& config)
    : config_(config)
    , enabled_(config.enabled)
    , nextAnimationId_(1)
{
}

void AnimationSystem::update(float deltaTime) {
    if (!enabled_) {
        return;
    }
    
    // Update all animations
    for (auto& pair : animations_) {
        pair.second->update(deltaTime);
    }
    
    // Clean up finished animations
    cleanupFinishedAnimations();
}

int AnimationSystem::animateFloat(float& target, float endValue, 
                                  float duration, EasingType easing) {
    if (!enabled_) {
        target = endValue;
        return -1;
    }
    
    float effectiveDuration = getEffectiveDuration(duration);
    EasingType effectiveEasing = getEffectiveEasing(easing);
    
    int id = nextAnimationId_++;
    animations_[id] = std::make_unique<FloatAnimation>(
        target, target, endValue, effectiveDuration, effectiveEasing);
    
    return id;
}

int AnimationSystem::animateVector2f(sf::Vector2f& target, const sf::Vector2f& endValue,
                                     float duration, EasingType easing) {
    if (!enabled_) {
        target = endValue;
        return -1;
    }
    
    float effectiveDuration = getEffectiveDuration(duration);
    EasingType effectiveEasing = getEffectiveEasing(easing);
    
    int id = nextAnimationId_++;
    animations_[id] = std::make_unique<Vector2fAnimation>(
        target, target, endValue, effectiveDuration, effectiveEasing);
    
    return id;
}

int AnimationSystem::animateColor(sf::Color& target, const sf::Color& endValue,
                                  float duration, EasingType easing) {
    if (!enabled_) {
        target = endValue;
        return -1;
    }
    
    float effectiveDuration = getEffectiveDuration(duration);
    EasingType effectiveEasing = getEffectiveEasing(easing);
    
    int id = nextAnimationId_++;
    animations_[id] = std::make_unique<ColorAnimation>(
        target, target, endValue, effectiveDuration, effectiveEasing);
    
    return id;
}

void AnimationSystem::stopAnimation(int animationId) {
    animations_.erase(animationId);
}

void AnimationSystem::stopAllAnimations() {
    animations_.clear();
}

bool AnimationSystem::isAnimationRunning(int animationId) const {
    auto it = animations_.find(animationId);
    return it != animations_.end() && !it->second->isFinished();
}

std::size_t AnimationSystem::getActiveAnimationCount() const {
    return animations_.size();
}

void AnimationSystem::cleanupFinishedAnimations() {
    auto it = animations_.begin();
    while (it != animations_.end()) {
        if (it->second->isFinished()) {
            it = animations_.erase(it);
        } else {
            ++it;
        }
    }
}

float AnimationSystem::getEffectiveDuration(float requestedDuration) const {
    return (requestedDuration > 0.0f) ? requestedDuration : config_.duration;
}

EasingType AnimationSystem::getEffectiveEasing(EasingType requestedEasing) const {
    return (requestedEasing != EasingType::Linear) ? requestedEasing : config_.easing;
}

} // namespace graphics
} // namespace nnv
