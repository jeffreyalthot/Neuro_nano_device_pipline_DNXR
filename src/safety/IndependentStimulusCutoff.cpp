#include "IndependentStimulusCutoff.h"

#include <algorithm>

namespace dnxr {

StimulusCutoffDecision IndependentStimulusCutoff::zeroAllOutputs(
    const SafeNeuralStimulusCommand& requested,
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

} // namespace dnxr
