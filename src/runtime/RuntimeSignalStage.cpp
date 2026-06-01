#include "RuntimeSignalStage.h"

namespace dnxr {

const char* RuntimeSignalStage::name() {
    return "signal";
}

RuntimeStageStatus RuntimeSignalStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

} // namespace dnxr
