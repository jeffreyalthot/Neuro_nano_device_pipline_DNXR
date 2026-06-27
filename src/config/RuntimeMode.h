#pragma once

#include <string>

namespace dnxr {

enum class RuntimeMode {
    Standard,
    Developer,
    FullUnlocked,
    Diagnostic,
    Simulation,
    Replay,
    Benchmark,
    SafeTest
};

std::string toString(RuntimeMode mode);
RuntimeMode runtimeModeFromString(const std::string& value);
bool isInspectionEnabled(RuntimeMode mode) noexcept;
bool isSyntheticExecutionAllowed(RuntimeMode mode) noexcept;

} // namespace dnxr
