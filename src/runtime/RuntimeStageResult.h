#pragma once

#include "RuntimeStageTypes.h"

namespace dnxr {

struct RuntimeStageResult {
    RuntimeStageStatus status = RuntimeStageStatus::ok("stage");
};

} // namespace dnxr
