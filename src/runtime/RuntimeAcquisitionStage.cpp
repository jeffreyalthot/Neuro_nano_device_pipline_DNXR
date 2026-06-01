#include "RuntimeAcquisitionStage.h"

namespace dnxr {

const char* RuntimeAcquisitionStage::name() {
    return "acquisition";
}

RuntimeStageStatus RuntimeAcquisitionStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

} // namespace dnxr
