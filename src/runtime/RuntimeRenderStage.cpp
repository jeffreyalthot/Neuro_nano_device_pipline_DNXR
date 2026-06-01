#include "RuntimeRenderStage.h"

namespace dnxr {

const char* RuntimeRenderStage::name() {
    return "render";
}

RuntimeStageStatus RuntimeRenderStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

} // namespace dnxr
