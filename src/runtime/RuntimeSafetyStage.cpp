#include "RuntimeSafetyStage.h"

namespace dnxr {

const char* RuntimeSafetyStage::name() {
    return "safety";
}

RuntimeStageStatus RuntimeSafetyStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

} // namespace dnxr
