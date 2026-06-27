#include "RuntimeMode.h"

#include <algorithm>

namespace dnxr {
namespace {
std::string normalized(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    std::replace(value.begin(), value.end(), '-', '_');
    return value;
}
} // namespace

std::string toString(RuntimeMode mode) {
    switch (mode) {
    case RuntimeMode::Standard: return "standard_mode";
    case RuntimeMode::Developer: return "developer_mode";
    case RuntimeMode::FullUnlocked: return "full_unlocked_mode";
    case RuntimeMode::Diagnostic: return "diagnostic_mode";
    case RuntimeMode::Simulation: return "simulation_mode";
    case RuntimeMode::Replay: return "replay_mode";
    case RuntimeMode::Benchmark: return "benchmark_mode";
    case RuntimeMode::SafeTest: return "safe_test_mode";
    }
    return "standard_mode";
}

RuntimeMode runtimeModeFromString(const std::string& value) {
    const auto mode = normalized(value);
    if (mode == "developer" || mode == "developer_mode" || mode == "dev") return RuntimeMode::Developer;
    if (mode == "full_unlocked" || mode == "full_unlocked_mode" || mode == "unlocked") return RuntimeMode::FullUnlocked;
    if (mode == "diagnostic" || mode == "diagnostic_mode") return RuntimeMode::Diagnostic;
    if (mode == "simulation" || mode == "simulation_mode") return RuntimeMode::Simulation;
    if (mode == "replay" || mode == "replay_mode") return RuntimeMode::Replay;
    if (mode == "benchmark" || mode == "benchmark_mode") return RuntimeMode::Benchmark;
    if (mode == "safe_test" || mode == "safe_test_mode") return RuntimeMode::SafeTest;
    return RuntimeMode::Standard;
}

bool isInspectionEnabled(RuntimeMode mode) noexcept {
    return mode == RuntimeMode::Developer || mode == RuntimeMode::FullUnlocked || mode == RuntimeMode::Diagnostic || mode == RuntimeMode::Benchmark || mode == RuntimeMode::SafeTest;
}

bool isSyntheticExecutionAllowed(RuntimeMode mode) noexcept {
    return mode == RuntimeMode::FullUnlocked || mode == RuntimeMode::Diagnostic || mode == RuntimeMode::Simulation || mode == RuntimeMode::Replay || mode == RuntimeMode::Benchmark || mode == RuntimeMode::SafeTest;
}

} // namespace dnxr
