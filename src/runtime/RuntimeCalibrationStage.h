#pragma once

#include "RuntimePipelineFrame.h"
#include "RuntimeStageTypes.h"
#include "../utils/RuntimeTraceRecorder.h"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace dnxr {

class RuntimeStageServices;

class RuntimeCalibrationStage {
public:
    struct Input {
        std::function<std::vector<float>(const std::vector<float>&, float, double)> calibrateForImmersion;
        std::function<UserBaselineProfile(const std::vector<float>&, float, double, const std::string&)> buildBaseline;
        std::function<void(const UserBaselineProfile&)> setBaseline;
        std::function<CalibrationDriftAssessment(const std::vector<float>&, float, float, float, double)> assessDrift;
        std::function<void()> resetStabilization;
        std::function<void(std::uint64_t, const std::string&, const std::string&, double, float, bool, const std::string&)> recordTrace;
        std::function<void(const std::string&)> logWarn;
        float userSensitivity = 1.05f;
        bool initializeBaseline = false;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, RuntimeStageServices& services);
};

} // namespace dnxr
