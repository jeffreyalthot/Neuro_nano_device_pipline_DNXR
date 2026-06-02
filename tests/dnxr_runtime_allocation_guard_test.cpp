#include "../src/runtime/RuntimeAllocationGuard.h"

#include <cassert>

int main() {
    dnxr::RuntimeAllocationGuard guard;
    dnxr::RuntimePipelineFrame frame;
    frame.rawNeuralSignals.reserve(8U);
    frame.cleanSignals.reserve(8U);
    frame.features.reserve(8U);
    frame.calibratedFeatures.reserve(8U);
    frame.sensoryFrame.reserve(8U);
    frame.telemetry.reserve(8U);
    frame.stimulusCommand.boundedChannels.reserve(8U);

    const auto baseline = guard.observe(frame);
    assert(!baseline.allocationGrowthSuspicious);
    assert(baseline.suspiciousFieldCount == 0U);

    const auto stable = guard.observe(frame);
    assert(!stable.allocationGrowthSuspicious);

    frame.sensoryFrame.reserve(16U);
    frame.stimulusCommand.boundedChannels.reserve(32U);
    const auto growth = guard.observe(frame);
    assert(growth.allocationGrowthSuspicious);
    assert(growth.suspiciousFieldCount == 2U);

    guard.reset();
    const auto afterReset = guard.observe(frame);
    assert(!afterReset.allocationGrowthSuspicious);
    return 0;
}
