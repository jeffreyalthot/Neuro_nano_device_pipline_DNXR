#include "ImmersionCore.h"

#include <algorithm>

#include "../utils/Logger.h"

namespace dnxr {

std::vector<float> ImmersionCore::buildImmersiveFrame(const std::vector<float>& calibratedSignal) {
    // This function maps calibrated neural intensity into bounded world-state rendering parameters.
    if (!calibratedSignal.empty()) {
        world_.intensity = std::clamp(calibratedSignal[0], 0.0F, 1.5F);
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
    return world_.safeExitCompleted;
}

bool ImmersionCore::isQuickMenuVisible() const {
    return world_.quickMenuVisible;
}

} // namespace dnxr
