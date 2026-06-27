#include "FullUnlockedConfig.h"

#include <sstream>

namespace dnxr {

FullUnlockedConfig FullUnlockedConfig::defaults() {
    FullUnlockedConfig config;
    config.enabledDiagnostics = {
        "runtime_state", "pipeline_steps", "latency", "frame_budget", "safety_decisions",
        "calibration_decisions", "feature_vectors", "replay_consistency", "benchmark_summary"};
    return config;
}

std::string FullUnlockedConfig::describe() const {
    std::ostringstream out;
    out << "FullUnlockedConfig{modules=" << enableAllSoftwareModules
        << ", internal_state=" << exposeInternalState
        << ", vectors=" << exposeVectorInspection
        << ", safety=" << exposeSafetyDecisions
        << ", simulation=" << enableSimulation
        << ", replay=" << enableReplay
        << ", benchmark=" << enableBenchmark
        << ", critical_interlocks=" << keepCriticalSafetyInterlocks
        << ", fail_on_silent_rejection=" << failOnSilentRejection
        << ", diagnostics=" << enabledDiagnostics.size() << "}";
    return out.str();
}

} // namespace dnxr
