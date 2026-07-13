#pragma once

#include "RuntimePipelineFrame.h"
#include "RuntimeCallback.h"
#include "RuntimeStageContext.h"
#include "RuntimeStageTypes.h"
#include "IBiometricSource.h"

#include <string>
#include <vector>

namespace dnxr {

class RuntimeStageServices;

class RuntimeAcquisitionStage {
public:
    struct Input {
        RuntimeCallback<BiometricSample(double)> readBiometricSample;
        RuntimeCallback<bool(const std::vector<float>&)> enforcePhysiologicalGuard;
        RuntimeCallback<void()> heartbeatRuntimeCycle;
        RuntimeCallback<void()> heartbeatAcquisition;
        RuntimeCallback<BciAcquisitionReadResult(const RuntimeStageContext&)> readCanonicalBciFrame;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(const RuntimeStageContext& context, const Input& input);
    static RuntimePipelineFrame execute(const RuntimeStageContext& context, RuntimeStageServices& services);
};

} // namespace dnxr
