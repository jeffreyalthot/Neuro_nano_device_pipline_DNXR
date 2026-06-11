#pragma once

#include "neural_chip/NeuralChipReplayRecorder.h"

#include <cstdint>
#include <string>

namespace dnxr {

struct PipelineSafetyCaseReport {
    std::uint64_t totalFrames = 0;
    std::uint64_t normalCommits = 0;
    std::uint64_t ackHolds = 0;
    std::uint64_t worldRejects = 0;
    std::uint64_t encoderRejects = 0;
    std::uint64_t capabilityHolds = 0;
    std::uint64_t realityAnchorHolds = 0;
    std::uint64_t lifecycleRejects = 0;
    std::uint64_t validatorRejects = 0;
    std::uint64_t emergencyLatches = 0;
    std::uint64_t controlledResumeFrames = 0;
    bool noDirectBusAccessPassed = false;
    bool noElectricalParamsPassed = false;
    bool noHumanReadyClaimsPassed = false;
    bool traceCoveragePassed = false;
    bool replayDigestPresent = false;
    std::string summary;
};

class PipelineSafetyCaseReportBuilder {
public:
    PipelineSafetyCaseReport build(const NeuralChipReplayRecorder& recorder,
                                   bool noDirectBusAccessPassed,
                                   bool noElectricalParamsPassed,
                                   bool noHumanReadyClaimsPassed,
                                   bool traceCoveragePassed) const;
};

} // namespace dnxr
