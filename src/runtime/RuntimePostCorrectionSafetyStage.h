#pragma once

#include "RuntimePipelineFrame.h"
#include "../full_dive/ExpectedPerceptionModel.h"
#include "../full_dive/PerceptionDivergenceDetector.h"
#include "../safety/FullDiveSafetySupervisor.h"

namespace dnxr {

class RuntimePostCorrectionSafetyStage {
public:
    static const char* name();
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame,
                                        const ExpectedPerceptionModel& expectedModel = ExpectedPerceptionModel{},
                                        const PerceptionDivergenceDetector& divergenceDetector = PerceptionDivergenceDetector{},
                                        const FullDiveSafetySupervisor& safetySupervisor = FullDiveSafetySupervisor{});
};

} // namespace dnxr
