#include "SensoryRenderer.h"

#include <algorithm>

namespace dnxr {

std::vector<float> SensoryRenderer::renderSensoryFrame(const VirtualWorldState& state) const {
    // This function merges visual-neural intensity channels with spatial audio channels.
    const float visualA = state.intensity;
    const float visualB = state.intensity * 0.8F;
    const float visualC = state.intensity * 0.6F;
    const float comfort = std::clamp(0.45F + 0.35F * state.safeExitComfortFade, 0.30F, 1.0F);
    auto audioFrame = audio_.synthesizeSpatialMix(state.audioSources, comfort, state.intensity, state.safeExitComfortFade);

    std::vector<float> frame{visualA, visualB, visualC};
    frame.insert(frame.end(), audioFrame.begin(), audioFrame.end());
    return frame;
}

} // namespace dnxr
