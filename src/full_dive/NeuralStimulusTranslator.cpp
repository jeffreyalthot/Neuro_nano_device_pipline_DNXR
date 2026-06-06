#include "NeuralStimulusTranslator.h"
#include <algorithm>
namespace dnxr {
NeuralStimulusTranslator::NeuralStimulusTranslator() = default;
NeuralStimulusTranslationResult NeuralStimulusTranslator::translate(const FullDiveNeuralFrame& frame) const {
    NeuralStimulusTranslationResult result;
    result.command.sequenceId = frame.sequenceId;
    result.command.monotonicTimestampMs = static_cast<std::int64_t>(frame.monotonicTimestampMs);
    result.command.calibrationProfileId = frame.safety.patientCalibrationHash;
    result.command.safetyMode = "typed_full_dive_closed_loop_locked";
    result.command.coveragePolicy = StimulusCoveragePolicy::FullMultisensoryFrame;
    result.command.requiredModalities = {SensoryModality::Visual, SensoryModality::Auditory, SensoryModality::HapticTouch, SensoryModality::Olfactory, SensoryModality::Gustatory, SensoryModality::Proprioceptive, SensoryModality::Vestibular};
    if (!frame.hasRequiredClinicalAnchors()) { result.reason = "missing_clinical_anchor_or_pain_block"; return result; }
    float sum = 0.0f;
    for (auto channel : frame.allChannels()) {
        channel.intensity = std::clamp(channel.intensity, 0.0f, channel.safetyLimit > 0.0f ? channel.safetyLimit : 1.0f);
        if (!targetMap_.validates(channel) && channel.modality != FullDiveModality::InteroceptiveSafety) { result.reason = "target_map_rejected_" + channel.targetRegion; return result; }
        result.command.boundedChannels.push_back(channel.intensity);
        sum += channel.intensity;
    }
    result.command.aggregateSensoryLoad = result.command.boundedChannels.empty() ? 0.0f : sum / static_cast<float>(result.command.boundedChannels.size());
    result.command.intensityScale = std::min(1.0f, result.command.aggregateSensoryLoad);
    result.command.riskScore = std::clamp(frame.safety.aggregateRisk, 0.0f, 1.0f);
    result.command.fiveSenseCoverage = true;
    result.command.allowStimulusDispatch = frame.safety.emergencyCutoffAvailable && result.command.riskScore < 0.8f;
    result.valid = result.command.allowStimulusDispatch;
    result.reason = result.valid ? "translated_and_hal_ready" : "translated_but_dispatch_locked";
    return result;
}
} // namespace dnxr
