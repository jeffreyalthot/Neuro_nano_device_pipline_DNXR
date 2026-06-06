#include "NeuralStimulusTranslator.h"

#include <algorithm>
#include <array>

namespace dnxr {
namespace {
std::size_t modalityIndex(FullDiveModality modality) {
    switch (modality) {
        case FullDiveModality::Visual: return static_cast<std::size_t>(SensoryModality::Visual);
        case FullDiveModality::Auditory: return static_cast<std::size_t>(SensoryModality::Auditory);
        case FullDiveModality::Somatosensory: return static_cast<std::size_t>(SensoryModality::HapticTouch);
        case FullDiveModality::Proprioceptive: return static_cast<std::size_t>(SensoryModality::Proprioceptive);
        case FullDiveModality::Vestibular: return static_cast<std::size_t>(SensoryModality::Vestibular);
        case FullDiveModality::Olfactory: return static_cast<std::size_t>(SensoryModality::Olfactory);
        case FullDiveModality::Gustatory: return static_cast<std::size_t>(SensoryModality::Gustatory);
        case FullDiveModality::InteroceptiveSafety:
        case FullDiveModality::RealityAnchor:
            return static_cast<std::size_t>(SensoryModality::Proprioceptive);
    }
    return 0U;
}
}

NeuralStimulusTranslator::NeuralStimulusTranslator() = default;

NeuralStimulusTranslationResult NeuralStimulusTranslator::translate(const FullDiveNeuralFrame& frame) const {
    NeuralStimulusTranslationResult result;
    result.command.sequenceId = frame.sequenceId;
    result.command.monotonicTimestampMs = static_cast<std::int64_t>(frame.monotonicTimestampMs);
    result.command.calibrationProfileId = frame.safety.patientCalibrationHash;
    result.command.safetyMode = "typed_full_dive_closed_loop_locked";
    result.command.coveragePolicy = StimulusCoveragePolicy::FullMultisensoryFrame;
    result.command.requiredModalities = {SensoryModality::Visual,
                                        SensoryModality::Auditory,
                                        SensoryModality::HapticTouch,
                                        SensoryModality::Olfactory,
                                        SensoryModality::Gustatory,
                                        SensoryModality::Proprioceptive,
                                        SensoryModality::Vestibular};
    result.command.modalityIntensities.assign(NeuralStimulusSafetyEnvelope::modalityCount(), 0.0f);

    if (!frame.hasRequiredClinicalAnchors()) {
        result.reason = "missing_clinical_anchor_or_pain_block";
        return result;
    }

    float sum = 0.0f;
    std::size_t count = 0U;
    bool rejected = false;
    std::string rejectReason;
    frame.forEachChannel([&](const NeuroChannelCommand& channel) {
        if (rejected) return;
        NeuroChannelCommand bounded = channel;
        bounded.intensity = std::clamp(bounded.intensity, 0.0f, bounded.safetyLimit > 0.0f ? bounded.safetyLimit : 1.0f);
        if (!targetMap_.validates(bounded) && bounded.modality != FullDiveModality::InteroceptiveSafety) {
            rejected = true;
            rejectReason = "target_map_rejected_" + bounded.targetRegion;
            return;
        }
        result.command.boundedChannels.push_back(bounded.intensity);
        const auto index = modalityIndex(bounded.modality);
        if (index < result.command.modalityIntensities.size()) {
            result.command.modalityIntensities[index] = std::max(result.command.modalityIntensities[index], bounded.intensity);
        }
        sum += bounded.intensity;
        ++count;
    });

    if (rejected) {
        result.reason = rejectReason;
        return result;
    }

    result.command.aggregateSensoryLoad = count == 0U ? 0.0f : sum / static_cast<float>(count);
    result.command.intensityScale = std::min(1.0f, result.command.aggregateSensoryLoad);
    result.command.riskScore = std::clamp(frame.safety.aggregateRisk, 0.0f, 1.0f);
    result.command.fiveSenseCoverage = true;
    result.command.coverageReason = "full_dive_frame_translated_without_channel_copy";
    result.command.allowStimulusDispatch = frame.safety.emergencyCutoffAvailable && result.command.riskScore < 0.8f && count > 0U;
    result.valid = result.command.allowStimulusDispatch;
    result.reason = result.valid ? "translated_and_hal_ready" : "translated_but_dispatch_locked";
    return result;
}

} // namespace dnxr
