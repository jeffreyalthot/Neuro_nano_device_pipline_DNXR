#include "neural_chip/SafeChipFrameHoldBuffer.h"

#include <algorithm>

namespace dnxr {

float SafeChipFrameHoldBuffer::clamp01(float value) { return std::max(0.0f, std::min(1.0f, value)); }

void SafeChipFrameHoldBuffer::applyHoldSafety(FullDiveChipFrame& frame) {
    frame.motorIntent = NeuralMotorIntentVector{};
    frame.motorIntent.stopIntent01 = 1.0f;
    frame.olfactory = NeuralOlfactoryVector{};
    frame.gustatory = NeuralGustatoryVector{};
    frame.vestibular.linearAcceleration01 = 0.0f;
    frame.vestibular.angularVelocity01 = 0.0f;
    frame.touch.painBlocked01 = 1.0f;
    frame.touch.burnBlocked01 = 1.0f;
    frame.touch.crushBlocked01 = 1.0f;
    frame.touch.shockBlocked01 = 1.0f;
    frame.realityAnchor = NeuralRealityAnchorVector{};
    frame.safety.simulationOnly = true;
    frame.safety.rawSensoryPayloadBlocked = true;
    frame.safety.painMappingBlocked = true;
    frame.safety.patientStopSignalAlive = true;
    frame.safety.emergencyCutoffAvailable = true;
    frame.safety.activeCeiling01 = DNXR_NEURAL_CHIP_ACTIVE_CEILING_01;
    frame.safety.aggregateRisk01 = std::max(frame.safety.aggregateRisk01, 0.20f);
}

void SafeChipFrameHoldBuffer::rememberAccepted(const FullDiveChipFrame& frame) {
    lastAccepted_ = frame;
    hasAccepted_ = true;
}

bool SafeChipFrameHoldBuffer::hasAcceptedFrame() const { return hasAccepted_; }

FullDiveChipFrame SafeChipFrameHoldBuffer::makeNeutralHoldFrame(std::uint64_t frameId, double timestampMs) const {
    FullDiveChipFrame out{};
    out.frameId = frameId;
    out.timestampMs = timestampMs;
    applyHoldSafety(out);
    return out;
}

FullDiveChipFrame SafeChipFrameHoldBuffer::makeDecompressionFrame(std::uint64_t frameId, double timestampMs, float ramp01) const {
    FullDiveChipFrame out = hasAccepted_ ? lastAccepted_ : FullDiveChipFrame{};
    const float r = clamp01(ramp01);
    out.frameId = frameId;
    out.timestampMs = timestampMs;
    out.visual.globalLuminance01 *= r;
    out.visual.contrast01 *= r;
    out.visual.edgeMap01 *= r;
    out.visual.motionFlow01 *= r;
    out.visual.objectSaliency01 *= r;
    out.visual.phospheneDensity01 *= r;
    out.auditory.loudness01 *= r;
    out.auditory.lowBand01 *= r;
    out.auditory.midBand01 *= r;
    out.auditory.highBand01 *= r;
    out.touch.contactPressure01 *= r;
    out.touch.contactArea01 *= r;
    out.touch.vibrationFlutter01 *= r;
    out.touch.vibrationTexture01 *= r;
    out.proprioception.avatarOwnership01 *= r;
    out.vestibular = NeuralVestibularVector{};
    out.olfactory = NeuralOlfactoryVector{};
    out.gustatory = NeuralGustatoryVector{};
    applyHoldSafety(out);
    return out;
}

} // namespace dnxr
