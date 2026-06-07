#include "RuntimeModalityPhaseLockStage.h"

#include <array>

namespace dnxr {
namespace {
constexpr std::size_t kTimingSlots = 8;
constexpr std::size_t idx(FullDiveModality modality) {
    return modality == FullDiveModality::Visual ? 0U :
           modality == FullDiveModality::Auditory ? 1U :
           modality == FullDiveModality::Somatosensory ? 2U :
           modality == FullDiveModality::Proprioceptive ? 3U :
           modality == FullDiveModality::Vestibular ? 4U :
           modality == FullDiveModality::Olfactory ? 5U :
           modality == FullDiveModality::Gustatory ? 6U : 7U;
}
}

const char* RuntimeModalityPhaseLockStage::name() { return "modality_phase_lock"; }

RuntimePipelineFrame RuntimeModalityPhaseLockStage::execute(RuntimePipelineFrame frame, const ModalityPhaseLockEngine& engine) {
    const float renderMs = static_cast<float>(frame.timestamp * 1000.0);
    const float halDispatchMs = static_cast<float>(frame.stimulusCommand.monotonicTimestampMs > 0 ? frame.stimulusCommand.monotonicTimestampMs : frame.fullDiveNeuralFrame.monotonicTimestampMs);
    const float ackMs = frame.feedbackChipFrame.monotonicTimestampMs > 0 ? static_cast<float>(frame.feedbackChipFrame.monotonicTimestampMs) : halDispatchMs;
    const float feedbackMs = frame.feedbackChipFrame.monotonicTimestampMs > 0 ? static_cast<float>(frame.feedbackChipFrame.monotonicTimestampMs) : ackMs;
    std::array<float, kTimingSlots> arrivals{};
    arrivals[idx(FullDiveModality::Visual)] = renderMs;
    arrivals[idx(FullDiveModality::Auditory)] = renderMs;
    arrivals[idx(FullDiveModality::Somatosensory)] = halDispatchMs;
    arrivals[idx(FullDiveModality::Proprioceptive)] = feedbackMs;
    arrivals[idx(FullDiveModality::Vestibular)] = halDispatchMs;
    arrivals[idx(FullDiveModality::Olfactory)] = halDispatchMs;
    arrivals[idx(FullDiveModality::Gustatory)] = halDispatchMs;
    arrivals[idx(FullDiveModality::RealityAnchor)] = renderMs;

    frame.phaseLockReport = engine.evaluate(frame.fullDiveNeuralFrame, arrivals);
    if (!frame.phaseLockReport.locked) {
        if (frame.phaseLockReport.worstSkewMs > 20.0f) {
            frame.fullDiveNeuralFrame.vestibular.channels.clear();
        }
        if (frame.phaseLockReport.worstSkewMs > 12.0f) {
            frame.fullDiveNeuralFrame.somatosensory.channels.clear();
            frame.fullDiveNeuralFrame.proprioception.channels.clear();
        }
        frame.fusedSafetyDecision.action = SafetyAction::RealityAnchor;
        frame.stimulusStatus = RuntimeStageStatus::degraded("modality_phase_lock_suppressed_real_timestamp_skew");
    }
    return frame;
}

} // namespace dnxr
