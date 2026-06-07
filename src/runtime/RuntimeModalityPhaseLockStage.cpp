#include "RuntimeModalityPhaseLockStage.h"

#include <map>

namespace dnxr {

const char* RuntimeModalityPhaseLockStage::name() { return "modality_phase_lock"; }

RuntimePipelineFrame RuntimeModalityPhaseLockStage::execute(RuntimePipelineFrame frame, const ModalityPhaseLockEngine& engine) {
    std::map<FullDiveModality, float> arrivals{{FullDiveModality::Visual, 16.0f},
                                               {FullDiveModality::Auditory, 12.0f},
                                               {FullDiveModality::Somatosensory, 8.0f},
                                               {FullDiveModality::Proprioceptive, 8.0f},
                                               {FullDiveModality::Vestibular, 6.0f},
                                               {FullDiveModality::Olfactory, 40.0f},
                                               {FullDiveModality::Gustatory, 40.0f},
                                               {FullDiveModality::RealityAnchor, 4.0f}};
    frame.phaseLockReport = engine.evaluate(frame.fullDiveNeuralFrame, arrivals);
    if (!frame.phaseLockReport.locked) {
        frame.fullDiveNeuralFrame.vestibular.channels.clear();
        frame.fullDiveNeuralFrame.somatosensory.channels.clear();
        frame.fusedSafetyDecision.action = SafetyAction::RealityAnchor;
        frame.stimulusStatus = RuntimeStageStatus::degraded("modality_phase_lock_suppressed_high_skew_channels");
    }
    return frame;
}

} // namespace dnxr
