#pragma once

#include "GameDesignBridge.h"
#include "SensoryRenderer.h"
#include <string>
#include <vector>

namespace dnxr {

class ImmersionCore {
public:
    std::vector<float> buildImmersiveFrame(const std::vector<float>& calibratedSignal);
    void handleUserQuickMenuInput(bool quickMenuButtonPressed, bool safeQuitSelected);
    bool isSafeExitReady() const;
    bool isQuickMenuVisible() const;
    const VirtualWorldState& currentWorldState() const;
    void updateUserCognitiveState(const std::string& inferredState);
    float comfortGate() const;

private:
    float computeImmersionGain() const;

    SensoryRenderer renderer_;
    GameDesignBridge gameBridge_;
    VirtualWorldState world_;
    std::string lastState_ = "state:neutral";
    float coherenceScore_ = 0.45F;
    float focusScore_ = 0.45F;
    float comfortScore_ = 0.75F;
};

} // namespace dnxr
