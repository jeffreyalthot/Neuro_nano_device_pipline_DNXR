#include "neural_chip/ChipCapabilityContract.h"

#include <algorithm>

namespace dnxr {

ChipCapabilityContract::ChipCapabilityContract(ChipCapabilities capabilities) : capabilities_(capabilities) {}

bool ChipCapabilityContract::requiresSafetyHold() const {
    return !capabilities_.supportsHardwareCutoff;
}

void ChipCapabilityContract::setCapabilities(ChipCapabilities capabilities) { capabilities_ = capabilities; }

FullDiveChipFrame ChipCapabilityContract::apply(const FullDiveChipFrame& frame) const {
    FullDiveChipFrame out = frame;
    if (!capabilities_.canVisual) out.visual = NeuralVisualVector{};
    if (!capabilities_.canAuditory) out.auditory = NeuralAuditoryVector{};
    if (!capabilities_.canHaptic) out.touch = NeuralTouchVector{};
    if (!capabilities_.canProprioceptive) out.proprioception = NeuralProprioceptionVector{};
    if (!capabilities_.canVestibular) out.vestibular = NeuralVestibularVector{};
    if (!capabilities_.canOlfactory) out.olfactory = NeuralOlfactoryVector{};
    if (!capabilities_.canGustatory) out.gustatory = NeuralGustatoryVector{};
    if (!capabilities_.supportsClosedLoopFeedback) {
        out.motorIntent.confidence01 = std::min(out.motorIntent.confidence01, 0.25f);
        out.safety.aggregateRisk01 = std::max(out.safety.aggregateRisk01, 0.50f);
    }
    if (!capabilities_.supportsHardwareCutoff) {
        out.safety.emergencyCutoffAvailable = false;
        out.safety.aggregateRisk01 = 1.0f;
    }
    out.safety.rawSensoryPayloadBlocked = out.safety.rawSensoryPayloadBlocked && true;
    return out;
}

} // namespace dnxr
