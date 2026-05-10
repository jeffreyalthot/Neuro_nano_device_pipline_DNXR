#include "ImmersionCore.h"

#include <algorithm>
#include <cmath>

#include "../utils/Logger.h"

namespace dnxr {

float ImmersionCore::computeImmersionGain() const {
    // This gain favors immersion when focus/coherence are high while staying bounded by comfort.
    const float presence = 0.55F * focusScore_ + 0.45F * coherenceScore_;
    const float comfortGate = std::clamp(comfortScore_, 0.2F, 1.0F);
    return std::clamp(0.65F + presence * comfortGate, 0.45F, 1.45F);
}

void ImmersionCore::updateUserCognitiveState(const std::string& inferredState) {
    // This function translates cognitive labels into rendering envelope coefficients.
    lastState_ = inferredState;

    if (inferredState.find("deep_immersion") != std::string::npos) {
        focusScore_ = 0.97F;
        coherenceScore_ = 0.93F;
        comfortScore_ = 0.80F;
    } else if (inferredState.find("focus") != std::string::npos) {
        focusScore_ = 0.90F;
        coherenceScore_ = 0.82F;
        comfortScore_ = 0.84F;
    } else if (inferredState.find("calm") != std::string::npos) {
        focusScore_ = 0.66F;
        coherenceScore_ = 0.80F;
        comfortScore_ = 0.92F;
    } else if (inferredState.find("stress") != std::string::npos ||
               inferredState.find("overload") != std::string::npos) {
        focusScore_ = 0.42F;
        coherenceScore_ = 0.35F;
        comfortScore_ = 0.48F;
    } else {
        focusScore_ = 0.58F;
        coherenceScore_ = 0.55F;
        comfortScore_ = 0.74F;
    }

    Logger::info("User state update -> " + lastState_ +
                 " | focus=" + std::to_string(focusScore_) +
                 " coherence=" + std::to_string(coherenceScore_) +
                 " comfort=" + std::to_string(comfortScore_));
}

std::vector<float> ImmersionCore::buildImmersiveFrame(const std::vector<float>& calibratedSignal) {
    // This function maps calibrated neural intensity into bounded world-state rendering parameters.
    if (!calibratedSignal.empty()) {
        world_.intensity = std::clamp(calibratedSignal[0] * computeImmersionGain(), 0.0F, 1.5F);
    }

    // This function synchronizes environment motion phase with audio spatial response cues.
    world_.environmentSyncPhase = std::fmod(world_.environmentSyncPhase + world_.intensity * 0.15F, 6.2831853F);
    for (std::size_t i = 0; i < world_.audioSources.size(); ++i) {
        const float phaseShift = world_.environmentSyncPhase + static_cast<float>(i) * 0.9F;
        world_.audioSources[i].azimuthDegrees = std::sin(phaseShift) * 150.0F;
        world_.audioSources[i].baseLoudness = std::clamp(0.25F + 0.20F * std::fabs(std::cos(phaseShift)), 0.1F, 0.7F);
    }

    if (world_.safeExitRequested && !world_.safeExitCompleted) {
        // Smoothly reduce immersion intensity to lower sensory shock during transition out of simulation.
        constexpr float kComfortFadeStep = 0.1F;
        world_.safeExitComfortFade = std::max(0.0F, world_.safeExitComfortFade - kComfortFadeStep);
        world_.intensity *= world_.safeExitComfortFade;

        if (world_.safeExitComfortFade <= 0.01F) {
            world_.safeExitCompleted = true;
            Logger::info("Safe-exit comfort fade completed. User can leave simulation safely.");
        }
    }

    return renderer_.renderSensoryFrame(world_);
}

void ImmersionCore::handleUserQuickMenuInput(bool quickMenuButtonPressed, bool safeQuitSelected) {
    // This function manages quick menu visibility and triggers safe exit sequences.
    if (quickMenuButtonPressed) {
        world_.quickMenuVisible = !world_.quickMenuVisible;
        Logger::info("Quick menu toggled from left visual corner circular button (three dots).");
    }

    if (world_.quickMenuVisible && safeQuitSelected && !world_.safeExitRequested) {
        world_.safeExitRequested = true;
        world_.safeExitComfortFade = 1.0F;
        Logger::info("Safe quit selected from quick menu. Starting progressive neural comfort fade.");
    }
}

bool ImmersionCore::isSafeExitReady() const {
    // This function reports whether all exit comfort conditions have been satisfied.
    return world_.safeExitCompleted;
}

bool ImmersionCore::isQuickMenuVisible() const {
    // This function reports whether the quick menu is currently visible in the headset.
    return world_.quickMenuVisible;
}

const VirtualWorldState& ImmersionCore::currentWorldState() const {
    // This function exposes the current world state snapshot for rendering and diagnostics.
    return world_;
}

float ImmersionCore::comfortGate() const {
    // This function exposes a bounded comfort coefficient for external calibration stages.
    return std::clamp(comfortScore_ * world_.safeExitComfortFade, 0.2F, 1.0F);
}

} // namespace dnxr
