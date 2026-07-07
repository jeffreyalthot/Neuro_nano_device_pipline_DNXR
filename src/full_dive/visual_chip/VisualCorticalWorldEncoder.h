#pragma once

#include "neural_chip/NeuralChipFrame.h"

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
};

struct PatientVisualMap { float phospheneDensityLimit01 = 0.35f; float stability01 = 0.5f; };
struct VisualSafetyState { bool allowVisualStimulus = true; float flickerRisk01 = 0.0f; float fatigueRisk01 = 0.0f; float edgeGain = 1.0f; };
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
