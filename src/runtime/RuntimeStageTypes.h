#pragma once

#include <string>

namespace dnxr {

struct RuntimeStageStatus {
    bool continueCycle = true;
    bool emergencyActive = false;
    std::string reason = "stage_ok";

    static RuntimeStageStatus ok(const std::string& stage) {
        return {true, false, stage + "_ok"};
    }

    static RuntimeStageStatus degraded(const std::string& reason) {
        return {false, false, reason};
    }

    static RuntimeStageStatus emergency(const std::string& reason) {
        return {false, true, reason};
    }
};

} // namespace dnxr
