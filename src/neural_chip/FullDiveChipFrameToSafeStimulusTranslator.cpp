#include "neural_chip/FullDiveChipFrameToSafeStimulusTranslator.h"

#include <algorithm>
#include <cstdint>

namespace dnxr {

float FullDiveChipFrameToSafeStimulusTranslator::clamp01(float value) { return std::max(0.0f, std::min(1.0f, value)); }

void FullDiveChipFrameToSafeStimulusTranslator::append(SafeNeuralStimulusCommandFixed& command, float value) {
    if (command.boundedChannels.size() < DNXR_MAX_STIMULUS_CHANNELS) {
        command.boundedChannels.push_back(clamp01(value));
    }
}

SafeNeuralStimulusCommandFixed FullDiveChipFrameToSafeStimulusTranslator::translate(
    const FullDiveChipFrame& frame, FullDiveStimulusCoveragePolicy policy) const {
    SafeNeuralStimulusCommandFixed command;
    command.sequenceId = frame.frameId;
    command.monotonicTimestampMs = static_cast<std::int64_t>(frame.timestampMs);
    command.safetyMode = "simulation_hil_normalized_channels_only";
    command.coveragePolicy = policy == FullDiveStimulusCoveragePolicy::FullMultisensoryFrame
                                 ? StimulusCoveragePolicy::FullMultisensoryFrame
                                 : StimulusCoveragePolicy::RequiredModalities;
    command.coverageReason = "normalized_full_dive_frame_no_electrical_parameters";
    command.allowStimulusDispatch = false;
    command.exposureBudgetOk = frame.safety.simulationOnly && frame.safety.rawSensoryPayloadBlocked && frame.safety.painMappingBlocked;
    command.riskScore = clamp01(frame.safety.aggregateRisk01);
    command.intensityScale = clamp01(frame.safety.activeCeiling01);

    append(command, frame.visual.globalLuminance01); append(command, frame.visual.contrast01); append(command, frame.visual.motionFlow01);
    append(command, frame.auditory.loudness01); append(command, frame.auditory.lowBand01); append(command, frame.auditory.midBand01); append(command, frame.auditory.highBand01);
    append(command, frame.touch.contactPressure01); append(command, frame.touch.vibrationTexture01);
    append(command, frame.proprioception.avatarOwnership01); append(command, frame.vestibular.linearAcceleration01); append(command, frame.vestibular.angularVelocity01);
    append(command, frame.olfactory.odorPresence01); append(command, frame.gustatory.sweet01); append(command, frame.realityAnchor.externalWorldRecall01);

    command.aggregateSensoryLoad = clamp01((frame.visual.globalLuminance01 + frame.auditory.loudness01 + frame.touch.contactPressure01 +
                                            frame.vestibular.linearAcceleration01 + frame.olfactory.odorPresence01 + frame.gustatory.sweet01) / 6.0f);
    command.fiveSenseCoverage = policy == FullDiveStimulusCoveragePolicy::FullMultisensoryFrame;
    return command;
}

} // namespace dnxr
namespace dnxr {
SafeNeuralStimulusCommandFixed FullDiveChipFrameToSafeStimulusTranslator::translate(
    const StimulusBySpikeFrame& frame, FullDiveStimulusCoveragePolicy policy) const {
    SafeNeuralStimulusCommandFixed command;
    command.sequenceId = frame.frameId;
    command.monotonicTimestampMs = static_cast<std::int64_t>(frame.timestampMs);
    command.safetyMode = "stimulus_by_spike_middleware_brain_chip_final_distribution";
    command.coveragePolicy = policy == FullDiveStimulusCoveragePolicy::FullMultisensoryFrame ? StimulusCoveragePolicy::FullMultisensoryFrame : StimulusCoveragePolicy::RequiredModalities;
    command.coverageReason = "stimulus_by_spike_normalized_no_raw_hardware_parameters";
    command.allowStimulusDispatch = frame.allowDispatch;
    command.exposureBudgetOk = frame.exposureBudgetOk;
    command.riskScore = clamp01(frame.riskScore01);
    command.intensityScale = clamp01(frame.intensityScale01);
    command.aggregateSensoryLoad = clamp01(frame.aggregateSensoryLoad01);
    command.fiveSenseCoverage = frame.fiveSenseCoverage;
    for (std::size_t i = 0; i < frame.channelCount && i < frame.boundedChannels01.size(); ++i) append(command, frame.boundedChannels01[i]);
    for (std::size_t i = 0; i < command.modalityIntensities.size() && i < frame.modalityIntensity01.size(); ++i) command.modalityIntensities[i] = clamp01(frame.modalityIntensity01[i]);
    return command;
}
} // namespace dnxr
