#include "RuntimeFullDiveStage.h"

namespace dnxr {

const char* RuntimeFullDiveStage::name() {
    return "fulldive";
}

RuntimeStageStatus RuntimeFullDiveStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

} // namespace dnxr
