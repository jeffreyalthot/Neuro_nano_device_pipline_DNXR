#include "RuntimeAllocationGuard.h"

#include <algorithm>
#include <array>

namespace dnxr {
namespace {

bool growsBeyondBaseline(std::size_t previous, std::size_t current) {
    if (previous == 0U) {
        return false;
    }
    return current > previous;
}

} // namespace

RuntimeAllocationGuardReport RuntimeAllocationGuard::observe(const RuntimePipelineFrame& frame) {
    return observeSamples({
        {"rawNeuralSignals", frame.rawNeuralSignals.capacity()},
        {"cleanSignals", frame.cleanSignals.capacity()},
        {"features", frame.features.capacity()},
        {"calibratedFeatures", frame.calibratedFeatures.capacity()},
        {"sensoryFrame", frame.sensoryFrame.capacity()},
        {"telemetry", frame.telemetry.capacity()},
        {"boundedChannels", frame.stimulusCommand.boundedChannels.capacity()},
    });
}

void RuntimeAllocationGuard::reset() {
    rawNeuralSignalsCapacity_ = 0U;
    cleanSignalsCapacity_ = 0U;
    featuresCapacity_ = 0U;
    calibratedFeaturesCapacity_ = 0U;
    sensoryFrameCapacity_ = 0U;
    telemetryCapacity_ = 0U;
    boundedChannelsCapacity_ = 0U;
}

RuntimeAllocationGuardReport RuntimeAllocationGuard::observeSamples(const std::vector<CapacitySample>& samples) {
    std::array<std::size_t*, 7> baselines{
        &rawNeuralSignalsCapacity_,
        &cleanSignalsCapacity_,
        &featuresCapacity_,
        &calibratedFeaturesCapacity_,
        &sensoryFrameCapacity_,
        &telemetryCapacity_,
        &boundedChannelsCapacity_,
    };

    RuntimeAllocationGuardReport report;
    const std::size_t count = std::min(samples.size(), baselines.size());
    for (std::size_t i = 0U; i < count; ++i) {
        std::size_t& baseline = *baselines[i];
        if (growsBeyondBaseline(baseline, samples[i].capacity)) {
            report.allocationGrowthSuspicious = true;
            ++report.suspiciousFieldCount;
            report.suspiciousFields.push_back(samples[i].name);
        }
        baseline = std::max(baseline, samples[i].capacity);
    }
    return report;
}

} // namespace dnxr
