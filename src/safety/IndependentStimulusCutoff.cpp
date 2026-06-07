#include "IndependentStimulusCutoff.h"

#include <algorithm>

namespace dnxr {

StimulusCutoffDecision IndependentStimulusCutoff::zeroAllOutputs(
    const SafeNeuralStimulusCommandFixed& requested,
    const std::string& reason) const {
    StimulusCutoffDecision decision;
    decision.reason = reason.empty() ? "clinical_lockdown" : reason;
    decision.emergencyLatched = true;
    decision.command = requested;
    std::fill(decision.command.boundedChannels.begin(), decision.command.boundedChannels.end(), 0.0f);
    std::fill(decision.command.modalityIntensities.begin(), decision.command.modalityIntensities.end(), 0.0f);
    decision.command.aggregateSensoryLoad = 0.0f;
    decision.command.intensityScale = 0.0f;
    decision.command.riskScore = 1.0f;
    decision.command.allowStimulusDispatch = false;
    decision.outputZeroed = true;
    return decision;
}

LegacyStimulusCutoffDecision IndependentStimulusCutoff::zeroAllOutputs(
    const SafeNeuralStimulusCommand& requested,
    const std::string& reason) const {
    const auto fixed = zeroAllOutputs(toFixedCommand(requested), reason);
    LegacyStimulusCutoffDecision legacy;
    legacy.outputZeroed = fixed.outputZeroed;
    legacy.emergencyLatched = fixed.emergencyLatched;
    legacy.reason = fixed.reason;
    legacy.command = toLegacyCommand(fixed.command);
    return legacy;
}

} // namespace dnxr
