#pragma once

#include "RuntimePipelineFrame.h"
#include "../full_dive/ModalityPhaseLockEngine.h"

namespace dnxr {

class RuntimeModalityPhaseLockStage {
public:
    static const char* name();
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const ModalityPhaseLockEngine& engine = ModalityPhaseLockEngine{});
};

} // namespace dnxr
