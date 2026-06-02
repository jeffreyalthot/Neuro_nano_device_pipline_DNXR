#include "RuntimeAllocationGuard.h"

#include <algorithm>
#include <array>

namespace dnxr {
namespace {

bool growsBeyondBaseline(std::size_t previousBytes, std::size_t currentBytes) {
    if (previousBytes == 0U) {
        return false;
    }
    return currentBytes > previousBytes;
}

bool growsBeyondAllowedThreshold(std::size_t previousBytes,
                                 std::size_t currentBytes,
                                 const RuntimeAllocationGuardConfig& config) {
    if (!growsBeyondBaseline(previousBytes, currentBytes)) {
        return false;
    }

    const auto factorLimit = static_cast<std::size_t>(static_cast<double>(previousBytes) *
                                                      static_cast<double>(config.allowedGrowthFactorAfterWarmup));
    const std::size_t absoluteLimit = previousBytes + config.allowedAbsoluteGrowthBytes;
    return currentBytes > factorLimit && currentBytes > absoluteLimit;
}

bool growsCritically(std::size_t previousBytes,
                     std::size_t currentBytes,
                     const RuntimeAllocationGuardConfig& config) {
    if (!growsBeyondBaseline(previousBytes, currentBytes)) {
        return false;
    }

    const std::size_t doubled = previousBytes * 2U;
    const std::size_t absoluteCritical = previousBytes + (config.allowedAbsoluteGrowthBytes * 4U);
    return currentBytes > doubled && currentBytes > absoluteCritical;
}

} // namespace

RuntimeAllocationGuard::RuntimeAllocationGuard(RuntimeAllocationGuardConfig config)
    : config_(config) {}

RuntimeAllocationGuardReport RuntimeAllocationGuard::observe(const RuntimePipelineFrame& frame) {
    return observeSamples({
        {"rawNeuralSignals", frame.rawNeuralSignals.capacity(), sizeof(float)},
        {"cleanSignals", frame.cleanSignals.capacity(), sizeof(float)},
        {"features", frame.features.capacity(), sizeof(float)},
        {"calibratedFeatures", frame.calibratedFeatures.capacity(), sizeof(float)},
        {"sensoryFrame", frame.sensoryFrame.capacity(), sizeof(float)},
        {"telemetry", frame.telemetry.capacity(), sizeof(float)},
        {"boundedChannels", frame.stimulusCommand.boundedChannels.capacity(), sizeof(float)},
    });
}

void RuntimeAllocationGuard::reset() {
    observedCycles_ = 0U;
    rawNeuralSignalsCapacityBytes_ = 0U;
    cleanSignalsCapacityBytes_ = 0U;
    featuresCapacityBytes_ = 0U;
    calibratedFeaturesCapacityBytes_ = 0U;
    sensoryFrameCapacityBytes_ = 0U;
    telemetryCapacityBytes_ = 0U;
    boundedChannelsCapacityBytes_ = 0U;
}

RuntimeAllocationGuardReport RuntimeAllocationGuard::observeSamples(const std::vector<CapacitySample>& samples) {
    std::array<std::size_t*, 7> baselines{
        &rawNeuralSignalsCapacityBytes_,
        &cleanSignalsCapacityBytes_,
        &featuresCapacityBytes_,
        &calibratedFeaturesCapacityBytes_,
        &sensoryFrameCapacityBytes_,
        &telemetryCapacityBytes_,
        &boundedChannelsCapacityBytes_,
    };

    RuntimeAllocationGuardReport report;
    const bool warmupComplete = observedCycles_ >= config_.warmupCycles;
    const std::size_t count = std::min(samples.size(), baselines.size());
    for (std::size_t i = 0U; i < count; ++i) {
        std::size_t& baselineBytes = *baselines[i];
        const std::size_t currentBytes = samples[i].capacity * samples[i].elementSize;
        if (growsBeyondBaseline(baselineBytes, currentBytes)) {
            report.growthObserved = true;
        }
        if (warmupComplete && growsBeyondAllowedThreshold(baselineBytes, currentBytes, config_)) {
            report.growthSuspicious = true;
            ++report.suspiciousFieldCount;
            report.suspiciousFields.push_back(samples[i].name);
        }
        if (warmupComplete && growsCritically(baselineBytes, currentBytes, config_)) {
            report.growthCritical = true;
        }
        baselineBytes = std::max(baselineBytes, currentBytes);
    }

    report.allocationGrowthSuspicious = report.growthSuspicious;
    ++observedCycles_;
    return report;
}

} // namespace dnxr
