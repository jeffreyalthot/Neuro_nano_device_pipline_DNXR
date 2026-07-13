#pragma once

#include "neural_chip/NeuralChipFrame.h"

#include <cstdint>

namespace dnxr {

struct VirtualSceneFrame {
    float luminance01 = 0.0f;
    float contrast01 = 0.0f;
    float edgeDensity01 = 0.0f;
    float motionFlow01 = 0.0f;
    float nearObstacle01 = 0.0f;
    float midDepthStructure01 = 0.0f;
    float farDepthStructure01 = 0.0f;
    float salientObject01 = 0.0f;
    float navigationCorridor01 = 0.0f;
    float hazard01 = 0.0f;
    float avatarBody01 = 0.0f;
    float textSymbol01 = 0.0f;
    float nearObstacleDensity01 = 0.0f;
    float walkableCorridorConfidence01 = 0.0f;
    float horizonStability01 = 0.0f;
    float interactiveObjectPriority01 = 0.0f;
    float handVisibility01 = 0.0f;
    float bodyVisibility01 = 0.0f;
    float worldGazeAttention01 = 0.0f;
};

struct VisualTopologyConfig {
    std::uint32_t schemaVersion = 1;
    std::uint16_t gridColumns = 16;
    std::uint16_t gridRows = 16;
    bool onOffChannelsEnabled = true;
    bool chromaticChannelsEnabled = false;
    float foveationStrength01 = 0.50f;
    float surroundSuppression01 = 0.20f;
    float compressionRatio01 = 0.50f;
    bool validate() const;
};

struct PatientVisualMap {
    float phospheneDensityLimit01 = 0.35f;
    float stability01 = 0.5f;
    float retinotopicConfidence01 = 0.5f;
    VisualTopologyConfig topology;
};
struct VisualSafetyState {
    bool allowVisualStimulus = true;
    float flickerRisk01 = 0.0f;
    float flashPatternRisk01 = 0.0f;
    float luminanceSlewRisk01 = 0.0f;
    float contrastRisk01 = 0.0f;
    float motionFlowRisk01 = 0.0f;
    float phospheneStabilityRisk01 = 0.0f;
    float temporalCoherenceRisk01 = 0.0f;
    float fatigueRisk01 = 0.0f;
    float latencyRisk01 = 0.0f;
    float edgeGain = 1.0f;
};
struct VisualComplianceDecision {
    bool accepted = true;
    bool requiresHold = false;
    float normalizedLoad01 = 0.0f;
    const char* reason = "visual_qms_accept";
};

class VisualCorticalWorldEncoder {
public:
    NeuralVisualVector encode(const VirtualSceneFrame& scene,
                              const PatientVisualMap& map,
                              const VisualSafetyState& safety) const;
    VisualComplianceDecision validateEncoded(const NeuralVisualVector& visual) const;
};

} // namespace dnxr
