#include "neural_chip/PipelineResumeController.h"

namespace dnxr {

void PipelineResumeController::lock() { phase_ = ResumePhase::Locked; reducedGainSteps_ = 0; }
void PipelineResumeController::beginControlledResume() { phase_ = ResumePhase::MinimalHold; reducedGainSteps_ = 0; }
bool PipelineResumeController::normal() const { return phase_ == ResumePhase::Normal; }
bool PipelineResumeController::locked() const { return phase_ == ResumePhase::Locked; }

ResumeDecision PipelineResumeController::decide() {
    switch (phase_) {
        case ResumePhase::Locked: return {ResumePhase::Locked, false, 0.0f, "emergency latch locked"};
        case ResumePhase::MinimalHold: phase_ = ResumePhase::NeutralHold; return {ResumePhase::MinimalHold, false, 0.0f, "resume minimal hard hold"};
        case ResumePhase::NeutralHold: phase_ = ResumePhase::Decompression; return {ResumePhase::NeutralHold, false, 0.0f, "resume neutral hold"};
        case ResumePhase::Decompression: phase_ = ResumePhase::ReducedGain; return {ResumePhase::Decompression, false, 0.10f, "resume decompression"};
        case ResumePhase::ReducedGain:
            if (++reducedGainSteps_ >= 2) phase_ = ResumePhase::Normal;
            return {ResumePhase::ReducedGain, true, 0.25f + 0.25f * static_cast<float>(reducedGainSteps_ - 1), "resume reduced gain"};
        case ResumePhase::Normal: return {ResumePhase::Normal, true, 1.0f, "normal"};
    }
    return {ResumePhase::Locked, false, 0.0f, "invalid resume phase"};
}

} // namespace dnxr
