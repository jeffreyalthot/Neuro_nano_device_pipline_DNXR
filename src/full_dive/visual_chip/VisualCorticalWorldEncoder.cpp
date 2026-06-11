#include "full_dive/visual_chip/VisualCorticalWorldEncoder.h"

#include <algorithm>

namespace dnxr {
namespace { float bounded(float v) { return std::max(0.0f, std::min(DNXR_NEURAL_CHIP_ACTIVE_CEILING_01, v)); } }

NeuralVisualVector VisualCorticalWorldEncoder::encode(const VirtualSceneFrame& scene,
                                                       const PatientVisualMap& map,
                                                       const VisualSafetyState& safety) const {
    NeuralVisualVector out{};
    if (!safety.allowVisualStimulus) return out;
    out.globalLuminance01 = bounded(scene.luminance01);
    out.contrast01 = bounded(scene.contrast01);
    out.edgeMap01 = bounded(scene.edgeDensity01 * safety.edgeGain);
    out.motionFlow01 = bounded(scene.motionFlow01);
    out.depthNear01 = bounded(std::max(scene.nearObstacle01, scene.nearObstacleDensity01));
    out.depthMid01 = bounded(std::max(scene.midDepthStructure01, scene.walkableCorridorConfidence01));
    out.depthFar01 = bounded(std::max(scene.farDepthStructure01, scene.horizonStability01));
    out.objectSaliency01 = bounded(std::max(scene.salientObject01, scene.interactiveObjectPriority01));
    out.navigationCorridor01 = bounded(scene.navigationCorridor01);
    out.hazardMap01 = bounded(scene.hazard01);
    out.avatarBodyVisibility01 = bounded(std::max(scene.avatarBody01, scene.handVisibility01 + scene.bodyVisibility01));
    out.textSymbolPriority01 = bounded(scene.textSymbol01);
    out.phospheneDensity01 = bounded(map.phospheneDensityLimit01);
    out.phospheneStability01 = bounded(map.stability01);
    out.flickerRisk01 = std::max(0.0f, std::min(1.0f, safety.flickerRisk01));
    out.visualFatigueRisk01 = std::max(0.0f, std::min(1.0f, safety.fatigueRisk01));
    if (out.flickerRisk01 > 0.35f) {
        out.contrast01 *= 0.5f;
        out.phospheneDensity01 *= 0.5f;
        out.motionFlow01 *= 0.6f;
    }
    return out;
}

} // namespace dnxr
