#pragma once

namespace dnxr {

enum class ResumePhase { Locked, MinimalHold, NeutralHold, Decompression, ReducedGain, Normal };

struct ResumeDecision {
    ResumePhase phase = ResumePhase::Normal;
    bool allowNormalFrame = true;
    float globalGain01 = 1.0f;
    const char* reason = "normal";
};

class PipelineResumeController {
public:
    void lock();
    void beginControlledResume();
    ResumeDecision decide();
    bool normal() const;

private:
    ResumePhase phase_ = ResumePhase::Normal;
    int reducedGainSteps_ = 0;
};

} // namespace dnxr
