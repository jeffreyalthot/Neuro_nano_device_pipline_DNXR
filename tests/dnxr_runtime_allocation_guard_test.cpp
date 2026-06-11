#include "../src/runtime/RuntimeAllocationGuard.h"

#include <cassert>

int main() {
    dnxr::RuntimeAllocationGuard guard(dnxr::RuntimeAllocationGuardConfig{2U, 1.10f, 16U});
    dnxr::RuntimePipelineFrame frame;
    frame.rawNeuralSignals.reserve(8U);
    frame.cleanSignals.reserve(8U);
    frame.features.reserve(8U);
    frame.calibratedFeatures.reserve(8U);
    frame.sensoryFrame.reserve(8U);
    frame.telemetry.reserve(8U);
    frame.stimulusCommand.boundedChannels.reserve(8U);

    const auto baseline = guard.observe(frame);
    assert(!baseline.growthObserved);
    assert(!baseline.growthSuspicious);
    assert(!baseline.allocationGrowthSuspicious);
    assert(baseline.suspiciousFieldCount == 0U);

    frame.sensoryFrame.reserve(12U);
    const auto warmupGrowth = guard.observe(frame);
    assert(warmupGrowth.growthObserved);
    assert(!warmupGrowth.growthSuspicious);

    const auto stable = guard.observe(frame);
    assert(!stable.growthObserved);
    assert(!stable.growthSuspicious);

    frame.sensoryFrame.reserve(32U);
    frame.stimulusCommand.boundedChannels.reserve(64U);
    const auto growth = guard.observe(frame);
    assert(growth.growthObserved);
    assert(growth.growthSuspicious);
    assert(growth.allocationGrowthSuspicious);
    assert(growth.growthCritical);
    assert(growth.suspiciousFieldCount == 1U);

    guard.reset();
    const auto afterReset = guard.observe(frame);
    assert(!afterReset.growthObserved);
    assert(!afterReset.growthSuspicious);
    return 0;
}
