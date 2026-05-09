#pragma once

#include "SensoryRenderer.h"
#include <vector>
#include <string>

namespace dnxr {

class ImmersionCore {
public:
    /// Builds an immersive frame from calibrated neural features.
    std::vector<float> buildImmersiveFrame(const std::vector<float>& calibratedSignal);

    /// Handles runtime interaction with the in-view quick menu for safe user-controlled exits.
    void handleUserQuickMenuInput(bool quickMenuButtonPressed, bool safeQuitSelected);

    /// Returns true once the comfort fade has completed and the user can be detached safely.
    bool isSafeExitReady() const;

    /// Returns true when the quick menu is currently visible to the user.
    bool isQuickMenuVisible() const;

    /// Receives cognitive state labels and adapts immersion envelope in real time.
    void updateUserCognitiveState(const std::string& inferredState);

private:
    float computeImmersionGain() const;

    SensoryRenderer renderer_;
    VirtualWorldState world_;
    std::string lastState_ = "state:neutral";
    float coherenceScore_ = 0.45F;
    float focusScore_ = 0.45F;
    float comfortScore_ = 0.75F;
};

} // namespace dnxr
