#pragma once

#include "RuntimeStageTypes.h"
#include <string>
#include <vector>

namespace dnxr {

class RuntimeRenderStage {
public:
    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
};

} // namespace dnxr
