#include "RuntimeInferenceStage.h"

namespace dnxr {

const char* RuntimeInferenceStage::name() {
    return "inference";
}

RuntimeStageStatus RuntimeInferenceStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

} // namespace dnxr
