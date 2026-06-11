#include "full_dive/motor_chip/MotorIntentConfidenceGate.h"

#include <algorithm>

namespace dnxr {

float MotorIntentConfidenceGate::clamp01(float value) { return std::max(0.0f, std::min(1.0f, value)); }

NeuralMotorIntentVector MotorIntentConfidenceGate::apply(const NeuralMotorIntentVector& intent, float signalQuality01, MotorIntentConfidenceDecision* decision) const {
    NeuralMotorIntentVector out = intent;
    MotorIntentConfidenceDecision local{};
    const float signal = clamp01(signalQuality01);
    if (out.confidence01 < 0.20f || signal < 0.20f) {
        out.moveForward01 = out.moveBackward01 = out.strafeLeft01 = out.strafeRight01 = 0.0f;
        out.turnYaw01 = out.lookPitch01 = out.gaitIntent01 = 0.0f;
        out.stopIntent01 = 1.0f;
        local.lowConfidence = true;
        local.safetyPoseRequired = true;
        local.reason = "motor confidence too low";
    }
    if (out.tremorRisk01 > 0.50f) {
        out.gaitIntent01 *= 0.5f;
        out.turnYaw01 *= 0.5f;
        local.safetyPoseRequired = true;
        local.reason = "motor tremor risk damping";
    }
    if (out.spasmRisk01 > 0.40f) {
        out.moveForward01 = out.moveBackward01 = out.strafeLeft01 = out.strafeRight01 = 0.0f;
        out.gaitIntent01 = 0.0f;
        out.stopIntent01 = 1.0f;
        local.freezeAvatar = true;
        local.safetyPoseRequired = true;
        local.reason = "motor spasm risk freeze";
    }
    if (out.fatigueRisk01 > 0.70f) out.gaitIntent01 *= 0.5f;
    if (decision) *decision = local;
    return out;
}

} // namespace dnxr
