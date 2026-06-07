#include "ClosedLoopNeuralFeedbackReader.h"

#include <algorithm>
#include <array>

namespace dnxr {
namespace {
float channelAt(const NeuralChipFrame& r, std::size_t index, float fallback) {
    return index < r.channels.size() ? std::clamp(r.channels[index], 0.0f, 1.0f) : fallback;
}
}

ClosedLoopFeedback ClosedLoopNeuralFeedbackReader::read(const NeuralChipFrame& r, const SafeNeuralStimulusCommandFixed& s) const {
    ClosedLoopFeedback f;
    f.available = !r.channels.empty();
    const float baseConfidence = std::clamp(r.signalQuality * (1.0f - s.riskScore), 0.0f, 1.0f);
    f.perModality.frameId = r.sequenceId;
    f.perModality.visualResponseConfidence = channelAt(r, 0, baseConfidence);
    f.perModality.auditoryResponseConfidence = channelAt(r, 1, baseConfidence);
    f.perModality.hapticResponseConfidence = channelAt(r, 2, baseConfidence);
    f.perModality.thermalResponseConfidence = channelAt(r, 3, baseConfidence);
    f.perModality.proprioceptiveResponseConfidence = channelAt(r, 4, baseConfidence);
    f.perModality.vestibularResponseConfidence = channelAt(r, 5, baseConfidence);
    f.perModality.olfactoryResponseConfidence = channelAt(r, 6, baseConfidence);
    f.perModality.gustatoryResponseConfidence = channelAt(r, 7, baseConfidence);
    f.perModality.realityAnchorResponseConfidence = channelAt(r, 8, baseConfidence);
    f.perModality.patientStopResponseConfidence = channelAt(r, 9, 1.0f);
    f.perModality.physiologicalAgreement = baseConfidence;
    f.perModality.patientSignalAgreement = f.perModality.patientStopResponseConfidence;
    f.perModality.safetyVerified = f.available && f.perModality.patientStopResponseConfidence > 0.5f && f.perModality.realityAnchorResponseConfidence > 0.25f;

    ClosedLoopFeedbackFrame expected = f.perModality;
    expected.visualResponseConfidence = s.modalityIntensities.size() > 0 ? 1.0f - s.modalityIntensities[0] * 0.25f : baseConfidence;
    expected.auditoryResponseConfidence = s.modalityIntensities.size() > 1 ? 1.0f - s.modalityIntensities[1] * 0.25f : baseConfidence;
    expected.hapticResponseConfidence = s.modalityIntensities.size() > 2 ? 1.0f - s.modalityIntensities[2] * 0.25f : baseConfidence;
    expected.proprioceptiveResponseConfidence = s.modalityIntensities.size() > 3 ? 1.0f - s.modalityIntensities[3] * 0.25f : baseConfidence;
    expected.vestibularResponseConfidence = s.modalityIntensities.size() > 4 ? 1.0f - s.modalityIntensities[4] * 0.25f : baseConfidence;
    expected.olfactoryResponseConfidence = s.modalityIntensities.size() > 5 ? 1.0f - s.modalityIntensities[5] * 0.25f : baseConfidence;
    expected.gustatoryResponseConfidence = s.modalityIntensities.size() > 6 ? 1.0f - s.modalityIntensities[6] * 0.25f : baseConfidence;

    PerModalityFeedbackAnalyzer analyzer;
    f.modalityAnalysis = analyzer.analyze(expected, f.perModality, f.perModality, f.perModality);
    f.perceptionConfidence = baseConfidence;
    f.adjustment = std::clamp(s.aggregateSensoryLoad - f.perceptionConfidence, -0.1f, 0.1f);
    f.safetyVerified = f.perModality.safetyVerified && f.modalityAnalysis.action != ClosedLoopFeedbackAction::EmergencyStop;
    return f;
}

} // namespace dnxr
