#pragma once

#include "RuntimePipelineFrame.h"

#include <cstddef>
#include <string>
#include <vector>

namespace dnxr {

struct RuntimeAllocationGuardReport {
    bool allocationGrowthSuspicious = false;
    std::size_t suspiciousFieldCount = 0U;
    std::vector<std::string> suspiciousFields;
};

class RuntimeAllocationGuard {
public:
    RuntimeAllocationGuardReport observe(const RuntimePipelineFrame& frame);
    void reset();

private:
    struct CapacitySample {
        const char* name;
        std::size_t capacity;
    };

    RuntimeAllocationGuardReport observeSamples(const std::vector<CapacitySample>& samples);

    std::size_t rawNeuralSignalsCapacity_ = 0U;
    std::size_t cleanSignalsCapacity_ = 0U;
    std::size_t featuresCapacity_ = 0U;
    std::size_t calibratedFeaturesCapacity_ = 0U;
    std::size_t sensoryFrameCapacity_ = 0U;
    std::size_t telemetryCapacity_ = 0U;
    std::size_t boundedChannelsCapacity_ = 0U;
};

} // namespace dnxr
