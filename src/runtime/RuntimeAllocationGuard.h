#pragma once

#include "RuntimePipelineFrame.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

struct RuntimeAllocationGuardConfig {
    std::uint64_t warmupCycles = 120U;
    float allowedGrowthFactorAfterWarmup = 1.10f;
    std::size_t allowedAbsoluteGrowthBytes = 4096U;
};

struct RuntimeAllocationGuardReport {
    bool growthObserved = false;
    bool growthSuspicious = false;
    bool growthCritical = false;
    bool allocationGrowthSuspicious = false;
    std::size_t suspiciousFieldCount = 0U;
    std::vector<std::string> suspiciousFields;
};

class RuntimeAllocationGuard {
public:
    explicit RuntimeAllocationGuard(RuntimeAllocationGuardConfig config = {});

    RuntimeAllocationGuardReport observe(const RuntimePipelineFrame& frame);
    void reset();

private:
    struct CapacitySample {
        const char* name;
        std::size_t capacity;
        std::size_t elementSize;
    };

    RuntimeAllocationGuardReport observeSamples(const std::vector<CapacitySample>& samples);

    RuntimeAllocationGuardConfig config_;
    std::uint64_t observedCycles_ = 0U;
    std::size_t rawNeuralSignalsCapacityBytes_ = 0U;
    std::size_t cleanSignalsCapacityBytes_ = 0U;
    std::size_t featuresCapacityBytes_ = 0U;
    std::size_t calibratedFeaturesCapacityBytes_ = 0U;
    std::size_t sensoryFrameCapacityBytes_ = 0U;
    std::size_t telemetryCapacityBytes_ = 0U;
    std::size_t boundedChannelsCapacityBytes_ = 0U;
};

} // namespace dnxr
