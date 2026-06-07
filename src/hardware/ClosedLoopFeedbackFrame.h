#pragma once

#include <cstdint>

namespace dnxr {

struct ClosedLoopFeedbackFrame {
    std::uint64_t frameId = 0;
    float visualResponseConfidence = 0.0f;
    float auditoryResponseConfidence = 0.0f;
    float hapticResponseConfidence = 0.0f;
    float thermalResponseConfidence = 0.0f;
    float proprioceptiveResponseConfidence = 0.0f;
    float vestibularResponseConfidence = 0.0f;
    float olfactoryResponseConfidence = 0.0f;
    float gustatoryResponseConfidence = 0.0f;
    float realityAnchorResponseConfidence = 0.0f;
    float patientStopResponseConfidence = 0.0f;
    float physiologicalAgreement = 0.0f;
    float patientSignalAgreement = 0.0f;
    bool safetyVerified = false;
};

} // namespace dnxr
