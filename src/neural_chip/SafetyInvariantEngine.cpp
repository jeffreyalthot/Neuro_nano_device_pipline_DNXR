#include "neural_chip/SafetyInvariantEngine.h"

#include <algorithm>

namespace dnxr {
namespace {
bool active(float v) { return v > 0.0001f; }
bool weakAnchor(const NeuralRealityAnchorVector& a) {
    return std::min({a.orientation01, a.identityContinuity01, a.sessionBoundary01, a.externalWorldRecall01}) < 0.50f;
}
}

SafetyInvariantResult SafetyInvariantEngine::validate(const FullDiveChipFrame& frame,
                                                       const NeuralChipSessionState& session,
                                                       const NeuralChipAckState& ack,
                                                       const ChipCapabilities& capabilities) const {
    if (!frame.safety.rawSensoryPayloadBlocked) return {false, "raw sensory payload is not blocked", true, true};
    if (!frame.safety.painMappingBlocked) return {false, "pain mapping is not blocked", true, true};
    if (session.emergencyStopLatched && frame.motorIntent.stopIntent01 < 1.0f) return {false, "emergency latch requires stop intent", true, true};
    if (ack.hasPendingAck && ack.timeout) return {false, "ack pending timeout forbids normal frame", false, true};
    if ((!capabilities.canOlfactory && (active(frame.olfactory.odorPresence01) || active(frame.olfactory.chemicalWarning01))) ||
        (!capabilities.canGustatory && (active(frame.gustatory.sweet01) || active(frame.gustatory.bitter01) || active(frame.gustatory.nauseaRisk01)))) {
        return {false, "chemical disabled modality is not neutral", false, true};
    }
    if (!capabilities.canVisual && active(frame.visual.flickerRisk01)) return {false, "visual disabled but flicker risk active", false, true};
    if ((!capabilities.canVestibular || weakAnchor(frame.realityAnchor)) &&
        (frame.vestibular.linearAcceleration01 > 0.20f || frame.vestibular.angularVelocity01 > 0.20f)) {
        return {false, "weak reality anchor forbids strong vestibular motion", false, true};
    }
    if (frame.safety.aggregateRisk01 > frame.safety.activeCeiling01) return {false, "aggregate risk exceeds active ceiling", false, true};
    return {};
}

} // namespace dnxr
