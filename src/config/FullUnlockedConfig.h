#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

struct FullUnlockedConfig {
    bool enableAllSoftwareModules = true;
    bool exposeInternalState = true;
    bool exposeVectorInspection = true;
    bool exposeSafetyDecisions = true;
    bool enableSimulation = true;
    bool enableReplay = true;
    bool enableBenchmark = true;
    bool keepCriticalSafetyInterlocks = true;
    bool failOnSilentRejection = true;
    std::uint64_t benchmarkCycles = 256;
    std::vector<std::string> enabledDiagnostics;

    static FullUnlockedConfig defaults();
    std::string describe() const;
};

} // namespace dnxr
