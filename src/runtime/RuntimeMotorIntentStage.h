#pragma once

#include "RuntimeMotorIntentTypes.h"
#include "RuntimePipelineFrame.h"
#include "../immersion/VirtualWorldDataIngress.h"

namespace dnxr {

class RuntimeMotorIntentStage {
public:
    static const char* name();
    static CausalMotorIntentStageResult decode(const RuntimePipelineFrame& frame,
                                               const CausalMotorIntentModelIdentity& model = {});
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame,
                                        IMotorWorldEgress* motorWorldEgress,
                                        const CausalMotorIntentModelIdentity& model = {});
};

} // namespace dnxr
