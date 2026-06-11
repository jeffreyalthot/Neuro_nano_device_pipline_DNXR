#include "neural_chip/NeuralChipSessionSupervisor.h"

#include <algorithm>

namespace dnxr {

float NeuralChipSessionSupervisor::clamp01(float value) { return std::max(0.0f, std::min(1.0f, value)); }

SessionSupervisorDecision NeuralChipSessionSupervisor::decide(const NeuralChipSessionState& state, const FullDiveChipFrame& frame) {
    SessionSupervisorDecision out{};
    const auto total = state.acceptedFrames + state.rejectedFrames + state.holdFrames;
    const float rejectedRatio = total ? static_cast<float>(state.rejectedFrames) / static_cast<float>(total) : 0.0f;
    const float holdRatio = total ? static_cast<float>(state.holdFrames) / static_cast<float>(total) : 0.0f;
    const float anchor = std::min({frame.realityAnchor.orientation01, frame.realityAnchor.identityContinuity01,
                                   frame.realityAnchor.sessionBoundary01, frame.realityAnchor.externalWorldRecall01});
    if (state.closedLoopStability01 < 0.50f) ++lowClosedLoopFrames_; else lowClosedLoopFrames_ = 0;
    if (anchor < 0.70f) return {false, true, true, 0.0f, "session supervisor reality anchor instability"};
    if (lowClosedLoopFrames_ >= 3) return {false, true, true, 0.0f, "session supervisor closed-loop instability"};
    if (state.sessionFatigue01 > 0.75f) return {true, true, false, 0.25f, "session supervisor fatigue hold"};
    if (holdRatio > 0.10f) return {true, true, false, 0.35f, "session supervisor hold ratio exceeded"};
    if (rejectedRatio > 0.20f || state.sessionRisk01 > 0.70f) return {true, false, false, 0.50f, "session supervisor global risk reduction"};
    return out;
}

FullDiveChipFrame NeuralChipSessionSupervisor::apply(const FullDiveChipFrame& frame, const SessionSupervisorDecision& decision) const {
    FullDiveChipFrame out = frame;
    if (!decision.reduceAllModalities) return out;
    const float g = clamp01(decision.globalGain01);
#define DNXR_GAIN(field) out.field *= g
    DNXR_GAIN(visual.globalLuminance01); DNXR_GAIN(visual.contrast01); DNXR_GAIN(visual.edgeMap01); DNXR_GAIN(visual.motionFlow01); DNXR_GAIN(visual.objectSaliency01); DNXR_GAIN(visual.phospheneDensity01);
    DNXR_GAIN(auditory.loudness01); DNXR_GAIN(auditory.lowBand01); DNXR_GAIN(auditory.midBand01); DNXR_GAIN(auditory.highBand01);
    DNXR_GAIN(touch.contactPressure01); DNXR_GAIN(touch.contactArea01); DNXR_GAIN(touch.vibrationFlutter01); DNXR_GAIN(touch.vibrationTexture01);
    DNXR_GAIN(vestibular.linearAcceleration01); DNXR_GAIN(vestibular.angularVelocity01);
    DNXR_GAIN(motorIntent.moveForward01); DNXR_GAIN(motorIntent.gaitIntent01); DNXR_GAIN(motorIntent.turnYaw01);
    out.olfactory = NeuralOlfactoryVector{};
    out.gustatory = NeuralGustatoryVector{};
#undef DNXR_GAIN
    return out;
}

void NeuralChipSessionSupervisor::reset() { lowClosedLoopFrames_ = 0; }

} // namespace dnxr
