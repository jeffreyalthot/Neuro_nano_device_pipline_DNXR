#include "ExpectedNeuralResponseModel.h"

#include <algorithm>

namespace dnxr {
namespace {
template <typename Channels>
float confidenceFor(const Channels& channels) {
    float maxValue = 0.0f;
    for (const auto& c : channels) maxValue = std::max(maxValue, c.intensity);
    return std::clamp(1.0f - maxValue * 0.25f, 0.0f, 1.0f);
}
}

ClosedLoopFeedbackFrame ExpectedNeuralResponseModel::expectedFromStimulus(const FullDiveNeuralFrame& frame) const {
    ClosedLoopFeedbackFrame f;
    f.frameId = frame.sequenceId;
    f.visualResponseConfidence = confidenceFor(frame.visual.channels);
    f.auditoryResponseConfidence = confidenceFor(frame.auditory.channels);
    f.hapticResponseConfidence = confidenceFor(frame.somatosensory.channels);
    f.proprioceptiveResponseConfidence = confidenceFor(frame.proprioception.channels);
    f.vestibularResponseConfidence = confidenceFor(frame.vestibular.channels);
    f.olfactoryResponseConfidence = confidenceFor(frame.smellTaste.olfactory.channels);
    f.gustatoryResponseConfidence = confidenceFor(frame.smellTaste.gustatory.channels);
    f.realityAnchorResponseConfidence = confidenceFor(frame.realityAnchor.channels);
    f.patientStopResponseConfidence = frame.interoceptiveSafety.patientStopSignalAlive ? 1.0f : 0.0f;
    f.physiologicalAgreement = 1.0f - std::clamp(frame.safety.aggregateRisk, 0.0f, 1.0f);
    f.patientSignalAgreement = f.patientStopResponseConfidence;
    f.safetyVerified = f.patientStopResponseConfidence > 0.9f && f.realityAnchorResponseConfidence > 0.5f;
    return f;
}

} // namespace dnxr
