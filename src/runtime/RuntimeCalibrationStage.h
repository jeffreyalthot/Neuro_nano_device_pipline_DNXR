#pragma once

#include "RuntimePipelineFrame.h"
#include "RuntimeCallback.h"
#include "RuntimeStageTypes.h"
#include "../utils/RuntimeTraceRecorder.h"

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

class RuntimeStageServices;

class RuntimeCalibrationStage {
public:
    struct Input {
        RuntimeCallback<std::vector<float>(const std::vector<float>&, float, double)> calibrateForImmersion;
        RuntimeCallback<UserBaselineProfile(const std::vector<float>&, float, double, const std::string&)> buildBaseline;
        RuntimeCallback<void(const UserBaselineProfile&)> setBaseline;
        RuntimeCallback<CalibrationDriftAssessment(const std::vector<float>&, float, float, float, double)> assessDrift;
        RuntimeCallback<void()> resetStabilization;
        RuntimeCallback<void(std::uint64_t, const std::string&, const std::string&, double, float, bool, const std::string&)> recordTrace;
        RuntimeCallback<void(const std::string&)> logWarn;
        float userSensitivity = 1.05f;
        bool initializeBaseline = false;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, RuntimeStageServices& services);
};

} // namespace dnxr
