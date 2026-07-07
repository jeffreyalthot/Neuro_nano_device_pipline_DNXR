#include "full_dive/visual_chip/VisualCorticalWorldEncoder.h"

#include <algorithm>

namespace dnxr {
namespace {
float bounded(float v) { return std::max(0.0f, std::min(DNXR_NEURAL_CHIP_ACTIVE_CEILING_01, v)); }
float unit(float v) { return std::max(0.0f, std::min(1.0f, v)); }
}

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
    out.flickerRisk01 = unit(safety.flickerRisk01);
    out.visualFatigueRisk01 = unit(safety.fatigueRisk01);
    if (out.flickerRisk01 > 0.35f) {
        out.contrast01 *= 0.5f;
        out.phospheneDensity01 *= 0.5f;
        out.motionFlow01 *= 0.6f;
    }
    return out;
}

VisualComplianceDecision VisualCorticalWorldEncoder::validateEncoded(
    const NeuralVisualVector& visual) const {
    VisualComplianceDecision decision{};
    const float peakStimulus = std::max({
        visual.globalLuminance01, visual.contrast01, visual.edgeMap01,
        visual.motionFlow01, visual.depthNear01, visual.depthMid01,
        visual.depthFar01, visual.objectSaliency01,
        visual.navigationCorridor01, visual.hazardMap01,
        visual.avatarBodyVisibility01, visual.textSymbolPriority01,
        visual.phospheneDensity01, visual.phospheneStability01});
    decision.normalizedLoad01 =
        unit(std::max(peakStimulus / DNXR_NEURAL_CHIP_ACTIVE_CEILING_01,
                      std::max(visual.flickerRisk01,
                               visual.visualFatigueRisk01)));
    if (peakStimulus > DNXR_NEURAL_CHIP_ACTIVE_CEILING_01) {
        decision.accepted = false;
        decision.requiresHold = true;
        decision.reason = "visual_active_ceiling_exceeded";
    } else if (visual.flickerRisk01 > 0.65f) {
        decision.accepted = false;
        decision.requiresHold = true;
        decision.reason = "visual_flicker_qms_hold";
    } else if (visual.visualFatigueRisk01 > 0.85f) {
        decision.accepted = false;
        decision.requiresHold = true;
        decision.reason = "visual_fatigue_qms_hold";
    }
    return decision;
}

} // namespace dnxr
