#include "full_dive/visual_chip/VisualCorticalWorldEncoder.h"
#include <cassert>

int main() {
    dnxr::VirtualSceneFrame scene;
    scene.nearObstacleDensity01 = 0.7f;
    scene.walkableCorridorConfidence01 = 0.6f;
    scene.horizonStability01 = 0.5f;
    scene.edgeDensity01 = 0.5f;
    scene.motionFlow01 = 0.8f;
    scene.interactiveObjectPriority01 = 0.7f;
    scene.handVisibility01 = 0.4f;
    scene.bodyVisibility01 = 0.4f;
    dnxr::VisualSafetyState safety;
    safety.edgeGain = 2.0f;
    safety.flickerRisk01 = 0.4f;
    dnxr::VisualCorticalWorldEncoder encoder;
    const auto visual = encoder.encode(scene, {}, safety);
    assert(visual.depthNear01 == 0.7f);
    assert(visual.edgeMap01 == dnxr::DNXR_NEURAL_CHIP_ACTIVE_CEILING_01);
    assert(visual.contrast01 == 0.0f);
    assert(visual.motionFlow01 < 0.8f);
    assert(visual.avatarBodyVisibility01 == dnxr::DNXR_NEURAL_CHIP_ACTIVE_CEILING_01);
    const auto accepted = encoder.validateEncoded(visual);
    assert(accepted.accepted);
    assert(!accepted.requiresHold);
    assert(accepted.normalizedLoad01 <= 1.0f);

    auto unsafe = visual;
    unsafe.flickerRisk01 = 0.9f;
    const auto blocked = encoder.validateEncoded(unsafe);
    assert(!blocked.accepted);
    assert(blocked.requiresHold);
    return 0;
}
