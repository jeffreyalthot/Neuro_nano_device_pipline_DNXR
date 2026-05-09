#pragma once

#include "SensoryRenderer.h"
#include <vector>

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

private:
    SensoryRenderer renderer_;
    VirtualWorldState world_;
};

} // namespace dnxr
