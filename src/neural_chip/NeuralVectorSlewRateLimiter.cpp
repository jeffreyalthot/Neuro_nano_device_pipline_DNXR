#include "neural_chip/NeuralVectorSlewRateLimiter.h"

#include <algorithm>

namespace dnxr {

NeuralVectorSlewRateLimiter::NeuralVectorSlewRateLimiter(NeuralVectorSlewPolicy policy) : policy_(policy) {}
float NeuralVectorSlewRateLimiter::clamp01(float value) { return std::max(0.0f, std::min(1.0f, value)); }
float NeuralVectorSlewRateLimiter::limit(float previous, float next, float maxDelta) {
    return clamp01(std::max(previous - maxDelta, std::min(previous + maxDelta, next)));
}

#define DNXR_LIMIT(prev, out, field, delta) out.field = limit(prev.field, out.field, delta)
FullDiveChipFrame NeuralVectorSlewRateLimiter::apply(const FullDiveChipFrame& frame) {
    return apply(frame, FullDiveDosePolicy{});
}

FullDiveChipFrame NeuralVectorSlewRateLimiter::apply(const FullDiveChipFrame& frame, const FullDiveDosePolicy& dosePolicy) {
    NeuralVectorSlewPolicy effective = policy_;
    effective.maxVisualDelta01 = std::min(effective.maxVisualDelta01, dosePolicy.visual.maxDeltaPerFrame01);
    effective.maxAuditoryDelta01 = std::min(effective.maxAuditoryDelta01, dosePolicy.auditory.maxDeltaPerFrame01);
    effective.maxTouchDelta01 = std::min(effective.maxTouchDelta01, dosePolicy.touch.maxDeltaPerFrame01);
    effective.maxVestibularDelta01 = std::min(effective.maxVestibularDelta01, dosePolicy.vestibular.maxDeltaPerFrame01);
    effective.maxMotorDelta01 = std::min(effective.maxMotorDelta01, dosePolicy.motor.maxDeltaPerFrame01);
    effective.maxChemicalDelta01 = std::min(effective.maxChemicalDelta01, std::min(dosePolicy.olfactory.maxDeltaPerFrame01, dosePolicy.gustatory.maxDeltaPerFrame01));
    FullDiveChipFrame out = frame;
    if (!hasPrevious_) {
        previous_ = out;
        hasPrevious_ = true;
        return out;
    }

    DNXR_LIMIT(previous_.visual, out.visual, globalLuminance01, effective.maxVisualDelta01); DNXR_LIMIT(previous_.visual, out.visual, contrast01, effective.maxVisualDelta01); DNXR_LIMIT(previous_.visual, out.visual, motionFlow01, effective.maxVisualDelta01); DNXR_LIMIT(previous_.visual, out.visual, phospheneDensity01, effective.maxVisualDelta01); DNXR_LIMIT(previous_.visual, out.visual, flickerRisk01, effective.maxVisualDelta01);
    DNXR_LIMIT(previous_.auditory, out.auditory, loudness01, effective.maxAuditoryDelta01); DNXR_LIMIT(previous_.auditory, out.auditory, lowBand01, effective.maxAuditoryDelta01); DNXR_LIMIT(previous_.auditory, out.auditory, midBand01, effective.maxAuditoryDelta01); DNXR_LIMIT(previous_.auditory, out.auditory, highBand01, effective.maxAuditoryDelta01); DNXR_LIMIT(previous_.auditory, out.auditory, startleRisk01, effective.maxAuditoryDelta01);
    DNXR_LIMIT(previous_.touch, out.touch, contactPressure01, effective.maxTouchDelta01); DNXR_LIMIT(previous_.touch, out.touch, contactArea01, effective.maxTouchDelta01); DNXR_LIMIT(previous_.touch, out.touch, vibrationFlutter01, effective.maxTouchDelta01); DNXR_LIMIT(previous_.touch, out.touch, vibrationTexture01, effective.maxTouchDelta01);
    DNXR_LIMIT(previous_.vestibular, out.vestibular, linearAcceleration01, effective.maxVestibularDelta01); DNXR_LIMIT(previous_.vestibular, out.vestibular, angularVelocity01, effective.maxVestibularDelta01); DNXR_LIMIT(previous_.vestibular, out.vestibular, motionSicknessRisk01, effective.maxVestibularDelta01);
    DNXR_LIMIT(previous_.motorIntent, out.motorIntent, moveForward01, effective.maxMotorDelta01); DNXR_LIMIT(previous_.motorIntent, out.motorIntent, gaitIntent01, effective.maxMotorDelta01); DNXR_LIMIT(previous_.motorIntent, out.motorIntent, confidence01, effective.maxMotorDelta01); DNXR_LIMIT(previous_.motorIntent, out.motorIntent, tremorRisk01, effective.maxMotorDelta01);
    DNXR_LIMIT(previous_.olfactory, out.olfactory, odorPresence01, effective.maxChemicalDelta01); DNXR_LIMIT(previous_.olfactory, out.olfactory, nauseaRisk01, effective.maxChemicalDelta01); DNXR_LIMIT(previous_.gustatory, out.gustatory, sweet01, effective.maxChemicalDelta01); DNXR_LIMIT(previous_.gustatory, out.gustatory, bitter01, effective.maxChemicalDelta01); DNXR_LIMIT(previous_.gustatory, out.gustatory, nauseaRisk01, effective.maxChemicalDelta01);

    out.frameId = frame.frameId;
    out.timestampMs = frame.timestampMs;
    out.safety = frame.safety;
    out.realityAnchor = frame.realityAnchor;
    out.touch.painBlocked01 = 1.0f; out.touch.burnBlocked01 = 1.0f; out.touch.crushBlocked01 = 1.0f; out.touch.shockBlocked01 = 1.0f;
    previous_ = out;
    return out;
}
#undef DNXR_LIMIT

void NeuralVectorSlewRateLimiter::reset() { previous_ = {}; hasPrevious_ = false; }

} // namespace dnxr
