#include "RuntimeStimulusStage.h"

namespace dnxr {

const char* RuntimeStimulusStage::name() {
    return "stimulus";
}

RuntimeStageStatus RuntimeStimulusStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

} // namespace dnxr
